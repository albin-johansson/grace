#include <algorithm>  // clamp
#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>   // cout, cerr
#include <limits>     // numeric_limits
#include <vector>     // vector

#include <grace/grace.hpp>
#include <vulkan/vulkan.h>

namespace gr = grace;

using uint32 = gr::uint32;
using uint64 = gr::uint64;

namespace {

inline constexpr uint32 kMaxU32 = std::numeric_limits<uint32>::max();
inline constexpr gr::ApiVersion kTargetVulkanVersion = {1, 2};
inline const std::vector kEnabledLayers = {"VK_LAYER_KHRONOS_validation"};

[[nodiscard]] auto create_instance(SDL_Window* window) -> gr::Instance
{
  const auto instance_extensions = gr::get_required_instance_extensions(window);

  VkResult result;
  if (auto instance = gr::Instance::make("Grace demo",
                                         kEnabledLayers,
                                         instance_extensions,
                                         {0, 1, 0},
                                         kTargetVulkanVersion,
                                         &result)) {
    std::cout << "Successfully created instance\n";
    return instance;
  }

  std::cerr << "Could not create instance: " << gr::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_surface(SDL_Window* window, VkInstance instance) -> gr::Surface
{
  if (auto surface = gr::Surface::make(window, instance)) {
    std::cout << "Successfully created surface\n";
    return surface;
  }

  std::cerr << "Could not create surface\n";
  return {};
}

[[nodiscard]] auto select_physical_device(VkInstance instance, VkSurfaceKHR surface)
    -> VkPhysicalDevice
{
  auto gpu_filter = [](VkPhysicalDevice gpu, VkSurfaceKHR surface) {
    const auto queue_family_indices = gr::get_queue_family_indices(gpu, surface);
    if (!queue_family_indices.present.has_value() ||
        !queue_family_indices.graphics.has_value()) {
      return false;
    }

    const auto swapchain_support = gr::get_swapchain_support(gpu, surface);
    if (swapchain_support.surface_formats.empty() ||
        swapchain_support.present_modes.empty()) {
      return false;
    }

    return true;
  };

  auto gpu_rater = [](VkPhysicalDevice gpu) {
    VkPhysicalDeviceProperties gpu_properties;
    vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

    int score = 1;

    if (gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score = 10;
    }
    else if (gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
      score = 0;
    }

    return score;
  };

  if (auto gpu = gr::pick_physical_device(instance, surface, gpu_filter, gpu_rater)) {
    VkPhysicalDeviceProperties gpu_properties;
    vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

    std::cout << "Selected GPU: " << gpu_properties.deviceName << '\n';
    return gpu;
  }

  std::cerr << "Could not select suitable physical device\n";
  return VK_NULL_HANDLE;
}

[[nodiscard]] auto create_device(VkPhysicalDevice gpu, VkSurfaceKHR surface) -> gr::Device
{
  // Example of structure extension for VkDeviceCreateInfo
  VkPhysicalDeviceDescriptorIndexingFeatures idx_features = {};
  idx_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  idx_features.descriptorBindingPartiallyBound = VK_TRUE;

  // Example of how to enable specific GPU features
  VkPhysicalDeviceFeatures enabled_features = {};
  enabled_features.samplerAnisotropy = VK_TRUE;
  enabled_features.fillModeNonSolid = VK_TRUE;

  // These are our required device extensions
  std::vector<const char*> extensions;
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef GRACE_USE_VULKAN_SUBSET
  extensions.push_back("VK_KHR_portability_subset");
#endif  // GRACE_USE_VULKAN_SUBSET

  // See also Device::make for even simpler factory functions
  const auto device_queue_infos = gr::make_device_queue_infos(gpu, surface);
  const auto device_info = gr::make_device_info(device_queue_infos.queues,
                                                kEnabledLayers,
                                                extensions,
                                                &enabled_features,
                                                &idx_features);

  VkResult result;
  if (auto device = gr::Device::make(gpu, device_info, &result)) {
    std::cout << "Successfully created logical device\n";
    return device;
  }

  std::cerr << "Could not create logical device: " << gr::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_allocator(VkInstance instance,
                                    VkPhysicalDevice gpu,
                                    VkDevice device) -> gr::UniqueAllocator
{
  VkResult result;
  if (auto allocator =
          make_allocator(instance, gpu, device, kTargetVulkanVersion, &result)) {
    std::cout << "Successfully created allocator\n";
    return allocator;
  }

  std::cerr << "Could not create allocator: " << gr::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_swapchain(SDL_Window* window,
                                    VkSurfaceKHR surface,
                                    VkPhysicalDevice gpu,
                                    VkDevice device,
                                    VmaAllocator allocator) -> gr::Swapchain
{
  const auto support = gr::get_swapchain_support(gpu, surface);

  VkExtent2D image_extent;
  if (support.surface_capabilities.currentExtent.width != kMaxU32) {
    image_extent = support.surface_capabilities.currentExtent;
  }
  else {
    int window_width = 0;
    int window_height = 0;
    SDL_GetWindowSizeInPixels(window, &window_width, &window_height);

    image_extent.width = std::clamp(static_cast<uint32>(window_width),
                                    support.surface_capabilities.minImageExtent.width,
                                    support.surface_capabilities.maxImageExtent.width);
    image_extent.height = std::clamp(static_cast<uint32>(window_height),
                                     support.surface_capabilities.minImageExtent.height,
                                     support.surface_capabilities.maxImageExtent.height);
  }

  // This is used to determine preferred swapchain image surface formats.
  auto surface_format_filter = [](const VkSurfaceFormatKHR& format) {
    if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format.format == VK_FORMAT_B8G8R8A8_SRGB ||
             format.format == VK_FORMAT_R8G8B8A8_SRGB;
    }

    return false;
  };

  // This is used to determine preferred swapchain present modes.
  auto present_mode_filter = [](const VkPresentModeKHR& mode) {
    return mode == VK_PRESENT_MODE_MAILBOX_KHR;
  };

  VkResult result = VK_SUCCESS;
  if (auto swapchain = gr::Swapchain::make(surface,
                                           gpu,
                                           device,
                                           allocator,
                                           image_extent,
                                           surface_format_filter,
                                           present_mode_filter,
                                           &result)) {
    std::cout << "Successfully created swapchain\n";
    return swapchain;
  }

  std::cerr << "Could not create swapchain: " << gr::to_string(result) << '\n';
  return {};
}

}  // namespace

int main(int, char**)
{
  const gr::SDL sdl;

  auto window = gr::Window::make("Grace demo", 800, 600);
  if (!window) {
    std::cerr << "Could not create OS window\n";
    return EXIT_FAILURE;
  }

  auto instance = create_instance(window);
  if (!instance) {
    return EXIT_FAILURE;
  }

  auto surface = create_surface(window, instance);
  if (!surface) {
    return EXIT_FAILURE;
  }

  VkPhysicalDevice gpu = select_physical_device(instance, surface);
  if (!gpu) {
    return EXIT_FAILURE;
  }

  auto device = create_device(gpu, surface);
  if (!device) {
    return EXIT_FAILURE;
  }

  const auto queue_family_indices = gr::get_queue_family_indices(gpu, surface);
  VkQueue graphics_queue = device.get_queue(queue_family_indices.graphics.value());
  VkQueue present_queue = device.get_queue(queue_family_indices.present.value());

  auto allocator = create_allocator(instance, gpu, device);
  if (!allocator) {
    return EXIT_FAILURE;
  }

  auto swapchain = create_swapchain(window, surface, gpu, device, allocator.get());
  if (!swapchain) {
    return EXIT_FAILURE;
  }

  // TODO render pass
  // TODO sampler
  // TODO pipeline cache
  // TODO pipeline
  // TODO command pool
  // TODO command buffer

  SDL_ShowWindow(window);

  bool running = true;
  while (running) {
    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
  }

  SDL_HideWindow(window);

  std::cout << "Exiting...\n";
  return EXIT_SUCCESS;
}
