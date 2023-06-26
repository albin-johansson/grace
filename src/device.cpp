#include "grace/device.hpp"

#include "grace/physical_device.hpp"

namespace grace {

void DeviceDeleter::operator()(VkDevice device) noexcept
{
  vkDestroyDevice(device, nullptr);
}

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

auto make_device(VkPhysicalDevice gpu,
                 VkSurfaceKHR surface,
                 const DeviceSpec& spec,
                 VkResult* result) -> UniqueDevice
{
  const auto unique_queue_families = get_unique_queue_family_indices(gpu, surface);

  std::vector<VkDeviceQueueCreateInfo> queue_infos;
  queue_infos.reserve(unique_queue_families.size());

  const float priority = 1.0f;
  for (const auto queue_family_index : unique_queue_families) {
    queue_infos.push_back(make_device_queue_info(queue_family_index, &priority));
  }

  const auto device_info = make_device_info(queue_infos,
                                            spec.layers,
                                            spec.extensions,
                                            spec.enabled_features,
                                            spec.next);

  VkDevice device = VK_NULL_HANDLE;
  const auto status = vkCreateDevice(gpu, &device_info, nullptr, &device);

  if (result) {
    *result = status;
  }

  return UniqueDevice {device};
}

}  // namespace grace
