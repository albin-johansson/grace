#include <cstdlib>   // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>  // cout, cerr
#include <vector>    // vector

#include <grace/grace.hpp>
#include <vulkan/vulkan.h>

using uint32 = grace::uint32;
using uint64 = grace::uint64;

namespace {

inline constexpr grace::ApiVersion kTargetVulkanVersion = {1, 2};
inline const std::vector kEnabledLayers = {"VK_LAYER_KHRONOS_validation"};

[[nodiscard]] auto create_instance(SDL_Window* window) -> grace::Instance
{
  const auto instance_extensions = grace::get_required_instance_extensions(window);

  VkResult result;
  if (auto instance = grace::Instance::make("Grace demo",
                                            kEnabledLayers,
                                            instance_extensions,
                                            {0, 1, 0},
                                            kTargetVulkanVersion,
                                            &result)) {
    std::cout << "Successfully created instance\n";
    return instance;
  }

  std::cerr << "Could not create instance: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_surface(SDL_Window* window, VkInstance instance)
    -> grace::Surface
{
  if (auto surface = grace::Surface::make(window, instance)) {
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

  if (auto gpu = grace::pick_physical_device(instance, surface, gpu_filter, gpu_rater)) {
    VkPhysicalDeviceProperties gpu_properties;
    vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

    std::cout << "Selected GPU: " << gpu_properties.deviceName << '\n';
    return gpu;
  }

  std::cerr << "Could not select suitable physical device\n";
  return VK_NULL_HANDLE;
}

[[nodiscard]] auto create_device(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> grace::Device
{
  // Example of structure extension for VkDeviceCreateInfo
  VkPhysicalDeviceDescriptorIndexingFeatures idx_features = {};
  idx_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  idx_features.descriptorBindingPartiallyBound = VK_TRUE;

  // Example of how to enable specific GPU features
  VkPhysicalDeviceFeatures enabled_features = {};
  enabled_features.samplerAnisotropy = VK_TRUE;
  enabled_features.fillModeNonSolid = VK_TRUE;

  // Define our required device extensions
  std::vector extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                            VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
                            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};
#ifdef GRACE_USE_VULKAN_SUBSET
  extensions.push_back("VK_KHR_portability_subset");
#endif  // GRACE_USE_VULKAN_SUBSET

  // See also Device::make for even simpler factory functions
  const auto device_queue_infos = grace::make_device_queue_infos(gpu, surface);
  const auto device_info = grace::make_device_info(device_queue_infos.queues,
                                                   kEnabledLayers,
                                                   extensions,
                                                   &enabled_features,
                                                   &idx_features);

  VkResult result;
  if (auto device = grace::Device::make(gpu, device_info, &result)) {
    std::cout << "Successfully created logical device\n";
    return device;
  }

  std::cerr << "Could not create logical device: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_allocator(VkInstance instance,
                                    VkPhysicalDevice gpu,
                                    VkDevice device) -> grace::UniqueAllocator
{
  VkResult result;
  if (auto allocator =
          make_allocator(instance, gpu, device, kTargetVulkanVersion, &result)) {
    std::cout << "Successfully created allocator\n";
    return allocator;
  }

  std::cerr << "Could not create allocator: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_swapchain(SDL_Window* window,
                                    VkSurfaceKHR surface,
                                    VkPhysicalDevice gpu,
                                    VkDevice device,
                                    VmaAllocator allocator) -> grace::Swapchain
{
  const auto support = grace::get_swapchain_support(gpu, surface);
  const auto image_extent =
      grace::pick_image_extent(window, support.surface_capabilities);

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
  if (auto swapchain = grace::Swapchain::make(surface,
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

  std::cerr << "Could not create swapchain: " << grace::to_string(result) << '\n';
  return {};
}

}  // namespace

int main(int, char**)
{
  const grace::SDL sdl;

  auto window = grace::Window::make("Grace demo", 800, 600);
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

  const auto queue_family_indices = grace::get_queue_family_indices(gpu, surface);
  VkQueue graphics_queue = device.get_queue(queue_family_indices.graphics.value());
  VkQueue present_queue = device.get_queue(queue_family_indices.present.value());
  if (graphics_queue == VK_NULL_HANDLE || present_queue == VK_NULL_HANDLE) {
    return EXIT_FAILURE;
  }

  auto allocator = create_allocator(instance, gpu, device);
  if (!allocator) {
    return EXIT_FAILURE;
  }

  auto swapchain = create_swapchain(window, surface, gpu, device, allocator.get());
  if (!swapchain) {
    return EXIT_FAILURE;
  }

  const auto subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  const auto main_subpass_access =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  const auto main_subpass_dependency =
      grace::make_subpass_dependency(VK_SUBPASS_EXTERNAL,
                                     0,
                                     subpass_stages,
                                     subpass_stages,
                                     0,
                                     main_subpass_access);

  grace::RenderPassBuilder render_pass_builder {device};

  VkResult render_pass_result = VK_ERROR_UNKNOWN;
  auto main_render_pass = render_pass_builder  //
                              .color_attachment(swapchain.info().image_format)
                              .depth_attachment(swapchain.get_depth_buffer_format())
                              .begin_subpass()
                              .set_color_attachment(0)
                              .set_depth_attachment(1)
                              .end_subpass()
                              .subpass_dependency(main_subpass_dependency)
                              .build(&render_pass_result);
  if (!main_render_pass) {
    std::cerr << "Could not create render pass: " << grace::to_string(render_pass_result)
              << '\n';
    return EXIT_FAILURE;
  }
  else {
    std::cout << "Successfully created main render pass\n";
  }

  VkResult sampler_result = VK_ERROR_UNKNOWN;
  auto sampler = grace::Sampler::make(device,
                                      gpu,
                                      VK_FILTER_LINEAR,
                                      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                      &sampler_result);
  if (!sampler) {
    std::cerr << "Could not create sampler: " << grace::to_string(sampler_result) << '\n';
    return EXIT_FAILURE;
  }
  else {
    std::cout << "Successfully created sampler\n";
  }

  VkResult pipeline_cache_result = VK_ERROR_UNKNOWN;
  auto pipeline_cache =
      grace::PipelineCache::make(device, nullptr, 0, 0, &pipeline_cache_result);
  if (!pipeline_cache) {
    std::cerr << "Could not create pipeline cache: "
              << grace::to_string(pipeline_cache_result) << '\n';
    return EXIT_FAILURE;
  }
  else {
    std::cout << "Successfully created pipeline cache\n";
  }

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
