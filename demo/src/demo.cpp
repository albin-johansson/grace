#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // exception
#include <iostream>   // cout, cerr
#include <stdexcept>  // runtime_error
#include <vector>     // vector

#include <grace/grace.hpp>
#include <vulkan/vulkan.h>

using uint32 = grace::uint32;
using usize = grace::usize;

namespace {

inline constexpr grace::ApiVersion kTargetVulkanVersion = {1, 2};
inline constexpr usize kMaxFramesInFlight = 2;

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
                                    VkDevice device) -> grace::Allocator
{
  VkResult result;
  if (auto allocator =
          grace::Allocator::make(instance, gpu, device, kTargetVulkanVersion, &result)) {
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

[[nodiscard]] auto create_render_pass(VkDevice device, const grace::Swapchain& swapchain)
    -> grace::RenderPass
{
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

  VkResult result = VK_ERROR_UNKNOWN;
  auto render_pass = grace::RenderPassBuilder {device}
                         .color_attachment(swapchain.info().image_format)
                         // TODO .depth_attachment(swapchain.get_depth_buffer_format())
                         .begin_subpass()
                         .use_color_attachment(0)
                         // TODO .set_depth_attachment(1)
                         .end_subpass()
                         .subpass_dependency(main_subpass_dependency)
                         .build(&result);
  if (render_pass) {
    std::cout << "Successfully created render pass\n";
    return render_pass;
  }

  std::cerr << "Could not create render pass: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_sampler(VkDevice device, VkPhysicalDevice gpu) -> grace::Sampler
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto sampler = grace::Sampler::make(device,
                                      gpu,
                                      VK_FILTER_LINEAR,
                                      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                      &result);
  if (sampler) {
    std::cout << "Successfully created sampler\n";
    return sampler;
  }

  std::cerr << "Could not create sampler: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_pipeline_cache(VkDevice device) -> grace::PipelineCache
{
  VkResult result = VK_ERROR_UNKNOWN;

  if (auto pipeline_cache = grace::PipelineCache::make(device, nullptr, 0, 0, &result)) {
    std::cout << "Successfully created pipeline cache\n";
    return pipeline_cache;
  }

  std::cerr << "Could not create pipeline cache: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_descriptor_set_layout(VkDevice device)
    -> grace::DescriptorSetLayout
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto descriptor_set_layout =
      grace::DescriptorSetLayoutBuilder {device}
          .use_push_descriptors()
          .allow_partially_bound_descriptors()
          .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
          .build(&result);

  if (descriptor_set_layout) {
    std::cout << "Successfully created descriptor set layout\n";
    return descriptor_set_layout;
  }

  std::cerr << "Could not create descriptor set layout: " << grace::to_string(result)
            << '\n';
  return {};
}

[[nodiscard]] auto create_pipeline_layout(VkDevice device,
                                          VkDescriptorSetLayout descriptor_set_layout)
    -> grace::PipelineLayout
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto pipeline_layout =
      grace::PipelineLayoutBuilder {device}
          .descriptor_set_layout(descriptor_set_layout)
          .push_constant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16)
          .build(&result);

  if (pipeline_layout) {
    std::cout << "Successfully created pipeline layout\n";
    return pipeline_layout;
  }

  std::cerr << "Could not create pipeline layout: " << grace::to_string(result) << '\n';
  return {};
}

[[nodiscard]] auto create_pipeline(VkDevice device,
                                   VkPipelineCache pipeline_cache,
                                   VkRenderPass render_pass) -> grace::GraphicsPipeline
{
  auto descriptor_set_layout = create_descriptor_set_layout(device);
  if (!descriptor_set_layout) {
    return {};
  }

  auto pipeline_layout = create_pipeline_layout(device, descriptor_set_layout);
  if (!pipeline_layout) {
    return {};
  }

  VkResult pipeline_result = VK_ERROR_UNKNOWN;
  auto pipeline =
      grace::GraphicsPipelineBuilder {device}
          .with_layout(pipeline_layout)
          .with_cache(pipeline_cache)
          .with_render_pass(render_pass, 0)
          .vertex_shader("assets/shaders/main.vert.spv")
          .fragment_shader("assets/shaders/main.frag.spv")
          .vertex_input_binding(0, 8 * sizeof(float))
          .vertex_attribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
          .vertex_attribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float))
          .vertex_attribute(0, 2, VK_FORMAT_R32G32_SFLOAT, 6 * sizeof(float))
          .color_blend_attachment(false)  // TODO need to track index
          .viewport(0, 0, 800, 600)
          .scissor(0, 0, 800, 600)
          .dynamic_state(VK_DYNAMIC_STATE_VIEWPORT)
          .dynamic_state(VK_DYNAMIC_STATE_SCISSOR)
          .build(&pipeline_result);
  if (pipeline) {
    std::cout << "Successfully created pipeline\n";
    return pipeline;
  }

  std::cerr << "Could not create pipeline: " << grace::to_string(pipeline_result) << '\n';
  return {};
}

[[nodiscard]] auto create_command_pool(VkDevice device, const uint32 queue_family_index)
    -> grace::CommandPool
{
  VkResult result = VK_ERROR_UNKNOWN;
  if (auto command_pool =
          grace::CommandPool::make(device,
                                   queue_family_index,
                                   VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                   &result)) {
    std::cout << "Successfully created command pool\n";
    return command_pool;
  }

  std::cerr << "Could not create command pool: " << grace::to_string(result) << '\n';
  return {};
}

}  // namespace

struct FrameData final {
  VkCommandBuffer command_buffer {VK_NULL_HANDLE};
  grace::Fence in_flight_fence;
  grace::Semaphore image_available_semaphore;
  grace::Semaphore render_finished_semaphore;
};

class GraceApplication final {
 public:
  GraceApplication()
  {
    mWindow = grace::Window::make("Grace demo", 800, 600);
    if (!mWindow) {
      throw std::runtime_error {"Could not create window"};
    }

    mInstance = create_instance(mWindow);
    if (!mInstance) {
      throw std::runtime_error {"Could not create instance"};
    }

    mSurface = create_surface(mWindow, mInstance);
    if (!mSurface) {
      throw std::runtime_error {"Could not create surface"};
    }

    mGPU = select_physical_device(mInstance, mSurface);
    if (!mGPU) {
      throw std::runtime_error {"Could not find suitable GPU"};
    }

    mDevice = create_device(mGPU, mSurface);
    if (!mDevice) {
      throw std::runtime_error {"Could not create logical device"};
    }

    const auto queue_family_indices = grace::get_queue_family_indices(mGPU, mSurface);
    mGraphicsQueue = mDevice.get_queue(queue_family_indices.graphics.value());
    mPresentQueue = mDevice.get_queue(queue_family_indices.present.value());
    if (mGraphicsQueue == VK_NULL_HANDLE || mPresentQueue == VK_NULL_HANDLE) {
      throw std::runtime_error {"Could not fetch device queues"};
    }

    mAllocator = create_allocator(mInstance, mGPU, mDevice);
    if (!mAllocator) {
      throw std::runtime_error {"Could not create allocator"};
    }

    mSwapchain = create_swapchain(mWindow, mSurface, mGPU, mDevice, mAllocator);
    if (!mSwapchain) {
      throw std::runtime_error {"Could not create swapchain"};
    }

    mRenderPass = create_render_pass(mDevice, mSwapchain);
    if (!mRenderPass) {
      throw std::runtime_error {"Could not create render pass"};
    }

    if (const auto result = _recreate_swapchain(); result != VK_SUCCESS) {
      std::cerr << "Could not recreate swapchain: " << grace::to_string(result) << '\n';
      throw std::runtime_error {"Could not prepare swapchain"};
    }

    mSampler = create_sampler(mDevice, mGPU);
    if (!mSampler) {
      throw std::runtime_error {"Could not create sampler"};
    }

    mPipelineCache = create_pipeline_cache(mDevice);
    if (!mPipelineCache) {
      throw std::runtime_error {"Could not create pipeline cache"};
    }

    mPipeline = create_pipeline(mDevice, mPipelineCache, mRenderPass);
    if (!mPipeline) {
      throw std::runtime_error {"Could not create pipeline"};
    }

    mCommandPool = create_command_pool(mDevice, queue_family_indices.graphics.value());
    if (!mCommandPool) {
      throw std::runtime_error {"Could not create command pool"};
    }

    VkResult command_buffer_result = VK_ERROR_UNKNOWN;
    auto command_buffers = grace::alloc_command_buffers(mDevice,
                                                        mCommandPool,
                                                        kMaxFramesInFlight,
                                                        &command_buffer_result);
    if (command_buffer_result != VK_SUCCESS) {
      std::cerr << "Could not allocate command buffers: "
                << grace::to_string(command_buffer_result) << '\n';
      throw std::runtime_error {"Could not allocate command buffers"};
    }

    for (usize index = 0; index < kMaxFramesInFlight; ++index) {
      auto& frame = mFrames.emplace_back();
      frame.command_buffer = command_buffers.at(index);

      frame.in_flight_fence = grace::Fence::make(mDevice, VK_FENCE_CREATE_SIGNALED_BIT);
      if (!frame.in_flight_fence) {
        throw std::runtime_error {"Could not create in_flight_fence"};
      }

      frame.image_available_semaphore = grace::Semaphore::make(mDevice);
      if (!frame.image_available_semaphore) {
        throw std::runtime_error {"Could not create image_available_semaphore"};
      }

      frame.render_finished_semaphore = grace::Semaphore::make(mDevice);
      if (!frame.render_finished_semaphore) {
        throw std::runtime_error {"Could not create render_finished_semaphore"};
      }
    }
  }

  void start()
  {
    mWindow.show();

    bool running = true;
    while (running) {
      SDL_Event event = {};
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        }
      }

      if (!running) {
        break;
      }

      // TODO add depth attachment and enable depth testing

      auto& frame = mFrames.at(mCurrentFrameIndex);

      // Wait until the GPU has finished executing previously submitted commands
      frame.in_flight_fence.wait();

      if (_begin_frame()) {
        frame.in_flight_fence.reset();

        _record_commands();
        _submit_commands();
        _present_image();

        mCurrentFrameIndex = (mCurrentFrameIndex + 1) % kMaxFramesInFlight;
      }
    }

    // Wait for the GPU to finish working so that we don't destroy any active resources.
    const auto wait_result = vkDeviceWaitIdle(mDevice);
    if (wait_result != VK_SUCCESS) {
      std::cerr << "Call to vkDeviceWaitIdle failed: " << grace::to_string(wait_result)
                << '\n';
    }

    mWindow.hide();
  }

 private:
  grace::SDL mSDL;
  grace::Window mWindow;
  grace::Instance mInstance;
  grace::Surface mSurface;
  VkPhysicalDevice mGPU {VK_NULL_HANDLE};
  grace::Device mDevice;
  VkQueue mGraphicsQueue {VK_NULL_HANDLE};
  VkQueue mPresentQueue {VK_NULL_HANDLE};
  grace::Allocator mAllocator;
  grace::Swapchain mSwapchain;
  grace::RenderPass mRenderPass;
  grace::Sampler mSampler;
  grace::PipelineCache mPipelineCache;
  grace::Pipeline mPipeline;
  grace::CommandPool mCommandPool;
  std::vector<FrameData> mFrames;
  usize mCurrentFrameIndex {0};
  bool mUseDepthBuffer {false};

  [[nodiscard]] auto _begin_frame() -> bool
  {
    auto& frame = mFrames.at(mCurrentFrameIndex);

    // Try to acquire an image from the swapchain
    const auto acquire_result =
        mSwapchain.acquire_next_image(frame.image_available_semaphore);

    if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
      std::cout << "Recreating outdated swapchain after image acquisition failed\n";

      const auto recreate_result = _recreate_swapchain();
      if (recreate_result != VK_SUCCESS) {
        std::cerr << "Failed to recreate swapchain: " << grace::to_string(recreate_result)
                  << '\n';
      }

      return false;
    }
    else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
      std::cerr << "Failed to acquire next swapchain image: "
                << grace::to_string(acquire_result) << '\n';
      throw false;
    }

    return true;
  }

  auto _recreate_swapchain() -> VkResult
  {
    auto window_size = mWindow.get_size_in_pixels();
    while (window_size.width == 0 || window_size.height == 0) {
      SDL_WaitEvent(nullptr);
      window_size = mWindow.get_size_in_pixels();
    }

    auto& swapchain_info = mSwapchain.info();
    swapchain_info.image_extent = window_size;

    std::cout << "New swapchain image extent: " << window_size.width << " x "
              << window_size.height << '\n';

    return mSwapchain.recreate(mRenderPass, mUseDepthBuffer);
  }

  void _record_commands()
  {
    auto& frame = mFrames.at(mCurrentFrameIndex);
    vkResetCommandBuffer(frame.command_buffer, 0);

    const auto cmd_buffer_begin_info = grace::make_command_buffer_begin_info();
    vkBeginCommandBuffer(frame.command_buffer, &cmd_buffer_begin_info);

    std::array<VkClearValue, 1> clear_values;
    clear_values[0].color = VkClearColorValue {0.0f, 0.0f, 0.0f, 1.0f};
    // TODO clear_values[0].depthStencil = VkClearDepthStencilValue {1.0f, 0};

    const auto image_extent = mSwapchain.info().image_extent;
    const auto render_pass_begin_info =
        grace::make_render_pass_begin_info(mRenderPass,
                                           mSwapchain.get_current_framebuffer(),
                                           {VkOffset2D {0, 0}, image_extent},
                                           clear_values.data(),
                                           grace::u32_size(clear_values));
    vkCmdBeginRenderPass(frame.command_buffer,
                         &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    const auto viewport =
        grace::make_viewport(0, 0, image_extent.width, image_extent.height);
    vkCmdSetViewport(frame.command_buffer, 0, 1, &viewport);

    const auto scissor =
        grace::make_rect_2d(0, 0, image_extent.width, image_extent.height);
    vkCmdSetScissor(frame.command_buffer, 0, 1, &scissor);

    vkCmdBindPipeline(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    // TODO render triangle

    vkCmdEndRenderPass(frame.command_buffer);
    vkEndCommandBuffer(frame.command_buffer);
  }

  void _submit_commands()
  {
    auto& frame = mFrames.at(mCurrentFrameIndex);

    // Submit our rendering commands to the graphics queue.
    // 1) Wait on the image_available_semaphore before executing the command buffer.
    // 2) Signal render_finished_semaphore and in_flight_fence after the commands
    //    have finished executing.
    VkSemaphore submit_wait_semaphores[] = {frame.image_available_semaphore};
    VkSemaphore submit_signal_semaphores[] = {frame.render_finished_semaphore};
    const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    const auto submit_info = grace::make_submit_info(&frame.command_buffer,
                                                     1,
                                                     submit_wait_semaphores,
                                                     1,
                                                     &wait_stage,
                                                     submit_signal_semaphores,
                                                     1);

    const auto submit_result =
        vkQueueSubmit(mGraphicsQueue, 1, &submit_info, frame.in_flight_fence);
    if (submit_result != VK_SUCCESS) {
      std::cerr << "Failed to submit commands to graphics queue: "
                << grace::to_string(submit_result) << '\n';
    }
  }

  void _present_image()
  {
    auto& frame = mFrames.at(mCurrentFrameIndex);

    const VkSemaphore present_wait_semaphores[] = {frame.render_finished_semaphore};
    const auto present_result =
        mSwapchain.present_image(mPresentQueue, present_wait_semaphores, 1);

    if (present_result == VK_ERROR_OUT_OF_DATE_KHR ||
        present_result == VK_SUBOPTIMAL_KHR) {
      std::cout << "Recreating outdated or suboptimal swapchain\n";
      _recreate_swapchain();
    }
    else if (present_result != VK_SUCCESS) {
      std::cerr << "Failed to present swapchain image: "
                << grace::to_string(present_result) << '\n';
    }
  }
};

auto main(int, char**) -> int
{
  try {
    GraceApplication application;
    application.start();
  }
  catch (const std::exception& e) {
    std::cerr << "Uncaught exception: " << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
