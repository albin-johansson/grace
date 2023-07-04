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
      .queueCreateInfoCount = static_cast<uint32>(queue_infos.size()),
      .pQueueCreateInfos = queue_infos.data(),
      .enabledLayerCount = static_cast<uint32>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
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

}  // namespace grace
