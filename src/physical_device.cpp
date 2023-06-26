#include "grace/physical_device.hpp"

#include <algorithm>      // max_element
#include <unordered_set>  // unordered_set

namespace grace {

auto get_physical_devices(VkInstance instance) -> std::vector<VkPhysicalDevice>
{
  uint32 gpu_count = 0;
  vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);

  std::vector<VkPhysicalDevice> gpus;
  gpus.resize(gpu_count);

  vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());

  return gpus;
}

auto get_extensions(VkPhysicalDevice gpu) -> std::vector<VkExtensionProperties>
{
  uint32 extension_count = 0;
  vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, nullptr);

  std::vector<VkExtensionProperties> extensions;
  extensions.resize(extension_count);

  vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, extensions.data());

  return extensions;
}

auto get_queue_families(VkPhysicalDevice gpu) -> std::vector<VkQueueFamilyProperties>
{
  uint32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families;
  queue_families.resize(queue_family_count);

  vkGetPhysicalDeviceQueueFamilyProperties(gpu,
                                           &queue_family_count,
                                           queue_families.data());

  return queue_families;
}

auto get_surface_formats(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> std::vector<VkSurfaceFormatKHR>
{
  uint32 format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, nullptr);

  std::vector<VkSurfaceFormatKHR> formats;
  formats.resize(format_count);

  vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, formats.data());

  return formats;
}

auto get_present_modes(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> std::vector<VkPresentModeKHR>
{
  uint32 present_mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr);

  std::vector<VkPresentModeKHR> present_modes;
  present_modes.resize(present_mode_count);

  vkGetPhysicalDeviceSurfacePresentModesKHR(gpu,
                                            surface,
                                            &present_mode_count,
                                            present_modes.data());

  return present_modes;
}

auto get_queue_family_indices(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> QueueFamilyIndices
{
  QueueFamilyIndices family_indices;

  const auto queue_families = get_queue_families(gpu);

  uint32 family_index = 0;
  for (const auto& queue_family : queue_families) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      family_indices.graphics = family_index;
    }

    VkBool32 has_present_support = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(gpu,
                                         family_index,
                                         surface,
                                         &has_present_support);

    if (has_present_support) {
      family_indices.present = family_index;
    }

    ++family_index;
  }

  return family_indices;
}

auto get_unique_queue_family_indices(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> std::vector<uint32>
{
  const auto queue_family_indices = get_queue_family_indices(gpu, surface);

  const auto graphics_family_index = queue_family_indices.graphics.value();
  const auto present_family_index = queue_family_indices.present.value();

  const std::unordered_set<uint32> unique_queue_families = {graphics_family_index,
                                                            present_family_index};

  return {unique_queue_families.begin(), unique_queue_families.end()};
}

auto get_swapchain_support(VkPhysicalDevice gpu, VkSurfaceKHR surface) -> SwapchainSupport
{
  SwapchainSupport support;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &support.surface_capabilities);

  support.surface_formats = get_surface_formats(gpu, surface);
  support.present_modes = get_present_modes(gpu, surface);

  return support;
}

struct SuitablePhysicalDevice final {
  VkPhysicalDevice gpu {VK_NULL_HANDLE};
  int score {};
};

auto pick_physical_device(VkInstance instance,
                          VkSurfaceKHR surface,
                          const PhysicalDeviceFilter& gpu_filter,
                          const PhysicalDeviceRater& gpu_rater) -> VkPhysicalDevice
{
  const auto gpus = get_physical_devices(instance);
  if (gpus.empty()) {
    return VK_NULL_HANDLE;
  }

  std::vector<SuitablePhysicalDevice> suitable_gpus;
  suitable_gpus.reserve(gpus.size());

  for (VkPhysicalDevice gpu : gpus) {
    if (gpu_filter(gpu, surface)) {
      const auto score = gpu_rater(gpu);
      suitable_gpus.push_back(SuitablePhysicalDevice {gpu, score});
    }
  }

  if (suitable_gpus.empty()) {
    return VK_NULL_HANDLE;
  }

  VkPhysicalDevice preferred_gpu = VK_NULL_HANDLE;

  auto gpu_comparer = [](const SuitablePhysicalDevice& a,
                         const SuitablePhysicalDevice& b) { return a.score < b.score; };
  preferred_gpu =
      std::max_element(suitable_gpus.begin(), suitable_gpus.end(), gpu_comparer)->gpu;

  return preferred_gpu;
}

}  // namespace grace
