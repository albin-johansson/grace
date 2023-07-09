/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "grace/device.hpp"

#include "grace/physical_device.hpp"

namespace grace {

auto make_device_queue_info(const uint32 queue_family_index, const float* priority)
    -> VkDeviceQueueCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = queue_family_index,
      .queueCount = 1,
      .pQueuePriorities = priority,
  };
}

auto make_device_queue_infos(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> DeviceQueueCreateInfos
{
  const auto unique_queue_families = get_unique_queue_family_indices(gpu, surface);

  DeviceQueueCreateInfos device_queue_infos;
  device_queue_infos.queues.reserve(unique_queue_families.size());
  device_queue_infos.priority = 1.0f;

  for (const auto queue_family_index : unique_queue_families) {
    device_queue_infos.queues.push_back(
        make_device_queue_info(queue_family_index, &device_queue_infos.priority));
  }

  return device_queue_infos;
}

auto make_device_info(const std::vector<VkDeviceQueueCreateInfo>& queue_infos,
                      const std::vector<const char*>& layers,
                      const std::vector<const char*>& extensions,
                      const VkPhysicalDeviceFeatures* enabled_features,
                      const void* next) -> VkDeviceCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = next,
      .flags = 0,
      .queueCreateInfoCount = u32_size(queue_infos),
      .pQueueCreateInfos = data_or_null(queue_infos),
      .enabledLayerCount = u32_size(layers),
      .ppEnabledLayerNames = data_or_null(layers),
      .enabledExtensionCount = u32_size(extensions),
      .ppEnabledExtensionNames = data_or_null(extensions),
      .pEnabledFeatures = enabled_features,
  };
}

void Device::DeviceDeleter::operator()(VkDevice device) noexcept
{
  vkDestroyDevice(device, nullptr);
}

auto Device::make(VkPhysicalDevice gpu,
                  const VkDeviceCreateInfo& device_info,
                  VkResult* result) -> Device
{
  VkDevice device_handle = VK_NULL_HANDLE;
  const auto status = vkCreateDevice(gpu, &device_info, nullptr, &device_handle);

  if (result) {
    *result = status;
  }

  if (status != VK_SUCCESS) {
    return {};
  }

  Device device;
  device.mDevice.reset(device_handle);

  return device;
}

auto Device::make(VkPhysicalDevice gpu,
                  VkSurfaceKHR surface,
                  const std::vector<const char*>& layers,
                  const std::vector<const char*>& extensions,
                  const VkPhysicalDeviceFeatures* enabled_features,
                  VkResult* result) -> Device
{
  const auto device_queue_infos = make_device_queue_infos(gpu, surface);
  const auto device_info =
      make_device_info(device_queue_infos.queues, layers, extensions, enabled_features);
  return Device::make(gpu, device_info, result);
}

auto Device::get_queue(const uint32 queue_family_index, const uint32 queue_index)
    -> VkQueue
{
  VkQueue queue = VK_NULL_HANDLE;
  vkGetDeviceQueue(mDevice.get(), queue_family_index, queue_index, &queue);
  return queue;
}

}  // namespace grace
