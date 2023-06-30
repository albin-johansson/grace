#include <cstdlib>   // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>  // cout, cerr
#include <vector>    // vector

#include <grace/grace.hpp>
#include <vulkan/vulkan.h>

namespace {

inline constexpr grace::ApiVersion kTargetVulkanVersion = {1, 2};
inline const std::vector kEnabledLayers = {"VK_LAYER_KHRONOS_validation"};

[[nodiscard]] auto create_instance(SDL_Window* window) -> grace::UniqueInstance
{
  const auto instance_extensions = grace::get_required_instance_extensions(window);

  VkResult result;
  auto instance = grace::make_instance("Grace demo",
                                       kEnabledLayers,
                                       instance_extensions,
                                       {0, 1, 0},
                                       kTargetVulkanVersion,
                                       &result);

  if (!instance) {
    std::cerr << "Could not create instance: " << result << '\n';
  }
  else {
    std::cout << "Successfully created instance\n";
  }

  return instance;
}

[[nodiscard]] auto create_surface(SDL_Window* window, VkInstance instance)
    -> std::optional<grace::Surface>
{
  auto surface = grace::make_surface(window, instance);

  if (!surface) {
    std::cerr << "Could not create surface\n";
  }
  else {
    std::cout << "Successfully created surface\n";
  }

  return surface;
}

[[nodiscard]] auto gpu_filter(VkPhysicalDevice gpu, VkSurfaceKHR surface) -> bool
{
  const auto queue_family_indices = grace::get_queue_family_indices(gpu, surface);
  if (!queue_family_indices.present.has_value() ||
      !queue_family_indices.graphics.has_value()) {
    return false;
  }

  const auto swapchain_support = grace::get_swapchain_support(gpu, surface);
  if (swapchain_support.surface_formats.empty() ||
      swapchain_support.present_modes.empty()) {
    return false;
  }

  return true;
}

[[nodiscard]] auto rate_gpu(VkPhysicalDevice gpu) -> int
{
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
}

[[nodiscard]] auto select_physical_device(VkInstance instance, VkSurfaceKHR surface)
    -> VkPhysicalDevice
{
  auto gpu = grace::pick_physical_device(instance, surface, &gpu_filter, &rate_gpu);

  if (gpu == VK_NULL_HANDLE) {
    std::cerr << "Could not select suitable physical device\n";
  }
  else {
    VkPhysicalDeviceProperties gpu_properties;
    vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

    std::cout << "Selected GPU: " << gpu_properties.deviceName << '\n';
  }

  return gpu;
}

[[nodiscard]] auto create_device(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> grace::UniqueDevice
{
  VkPhysicalDeviceDescriptorIndexingFeatures indexing_features = {};
  indexing_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  indexing_features.descriptorBindingPartiallyBound = VK_TRUE;

  VkPhysicalDeviceFeatures enabled_features = {};
  enabled_features.samplerAnisotropy = VK_TRUE;
  enabled_features.fillModeNonSolid = VK_TRUE;

  grace::DeviceSpec device_spec;
  device_spec.layers = kEnabledLayers;
  device_spec.enabled_features = &enabled_features;
  device_spec.next = &indexing_features;

#ifdef GRACE_USE_VULKAN_SUBSET
  device_spec.extensions.push_back("VK_KHR_portability_subset");
#endif  // GRACE_USE_VULKAN_SUBSET

  VkResult result;
  auto device = grace::make_device(gpu, surface, device_spec, &result);

  if (!device) {
    std::cerr << "Could not create logical device: " << result << '\n';
  }
  else {
    std::cout << "Successfully created logical device\n";
  }

  return device;
}

void get_device_queues(VkDevice device,
                       VkPhysicalDevice gpu,
                       VkSurfaceKHR surface,
                       VkQueue* graphics_queue,
                       VkQueue* present_queue)
{
  const auto queue_family_indices = grace::get_queue_family_indices(gpu, surface);

  if (graphics_queue) {
    vkGetDeviceQueue(device, queue_family_indices.graphics.value(), 0, graphics_queue);
  }

  if (present_queue) {
    vkGetDeviceQueue(device, queue_family_indices.present.value(), 0, present_queue);
  }
}

[[nodiscard]] auto create_allocator(VkInstance instance,
                                    VkPhysicalDevice gpu,
                                    VkDevice device) -> grace::UniqueAllocator
{
  VkResult result;
  auto allocator =
      grace::make_allocator(instance, gpu, device, kTargetVulkanVersion, &result);

  if (!allocator) {
    std::cerr << "Could not create allocator: " << result << '\n';
  }
  else {
    std::cout << "Successfully created allocator\n";
  }

  return allocator;
}

}  // namespace

int main(int, char**)
{
  const grace::SDL sdl;
  auto window = grace::make_window("Grace demo", 800, 600);

  auto instance = create_instance(window.get());
  if (!instance) {
    return EXIT_FAILURE;
  }

  auto surface = create_surface(window.get(), instance.get());
  if (!surface) {
    return EXIT_FAILURE;
  }

  VkPhysicalDevice gpu = select_physical_device(instance.get(), surface->ptr);
  if (!gpu) {
    return EXIT_FAILURE;
  }

  auto device = create_device(gpu, surface->ptr);
  if (!device) {
    return EXIT_FAILURE;
  }

  VkQueue graphics_queue = VK_NULL_HANDLE;
  VkQueue present_queue = VK_NULL_HANDLE;
  get_device_queues(device.get(), gpu, surface->ptr, &graphics_queue, &present_queue);

  auto allocator = create_allocator(instance.get(), gpu, device.get());
  if (!allocator) {
    return EXIT_FAILURE;
  }

  // TODO swapchain
  // TODO render pass
  // TODO sampler
  // TODO pipeline cache
  // TODO pipeline
  // TODO command pool
  // TODO command buffer

  SDL_ShowWindow(window.get());

  bool running = true;
  while (running) {
    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
  }

  SDL_HideWindow(window.get());

  std::cout << "Exiting...\n";
  return EXIT_SUCCESS;
}
