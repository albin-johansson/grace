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

#include "example.hpp"

#include <array>      // array
#include <stdexcept>  // runtime_error

namespace grace::examples {
namespace {

[[nodiscard]] auto _gpu_filter(VkPhysicalDevice gpu, VkSurfaceKHR surface) -> bool
{
  const auto queue_family_indices = get_queue_family_indices(gpu, surface);
  if (!queue_family_indices.present.has_value() ||
      !queue_family_indices.graphics.has_value()) {
    return false;
  }

  const auto swapchain_support = get_swapchain_support(gpu, surface);
  if (swapchain_support.surface_formats.empty() ||
      swapchain_support.present_modes.empty()) {
    return false;
  }

  return true;
}

[[nodiscard]] auto _gpu_rater(VkPhysicalDevice gpu) -> int
{
  VkPhysicalDeviceProperties gpu_properties;
  vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

  int score = 1;

  if (gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score = 10;
  }
  else if (gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
    score = -1;
  }

  return score;
};

[[nodiscard]] auto _swapchain_surface_format_filter(const VkSurfaceFormatKHR& format)
    -> bool
{
  if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
    return format.format == VK_FORMAT_B8G8R8A8_SRGB ||
           format.format == VK_FORMAT_R8G8B8A8_SRGB;
  }

  return false;
};

[[nodiscard]] auto _swapchain_present_mode_filter(const VkPresentModeKHR mode) -> bool
{
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
};

}  // namespace

Example::Example(const char* name)
    : mWindow {Window::make(name, 800, 600)}
{
  if (!mWindow) {
    throw std::runtime_error {"Could not create window"};
  }

  VkResult result = VK_ERROR_UNKNOWN;

  const auto instance_extensions = get_required_instance_extensions(mWindow);
  mInstance = Instance::make(name,
                             kEnabledLayers,
                             instance_extensions,
                             {0, 1, 0},
                             kTargetVulkanVersion,
                             &result);
  if (!mInstance) {
    std::cerr << "Could not create instance: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create instance"};
  }

  mSurface = Surface::make(mWindow, mInstance);
  if (!mSurface) {
    throw std::runtime_error {"Could not create surface"};
  }

  mGPU = pick_physical_device(mInstance, mSurface, &_gpu_filter, &_gpu_rater);
  if (!mGPU) {
    throw std::runtime_error {"Could not pick suitable GPU"};
  }

  const auto queue_family_indices = get_queue_family_indices(mGPU, mSurface);

  // Define our required device extensions
  std::vector device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                   VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
                                   VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};
#ifdef GRACE_USE_VULKAN_SUBSET
  device_extensions.push_back("VK_KHR_portability_subset");
#endif  // GRACE_USE_VULKAN_SUBSET

  VkPhysicalDeviceFeatures enabled_gpu_features = {};
  enabled_gpu_features.samplerAnisotropy = VK_TRUE;
  enabled_gpu_features.fillModeNonSolid = VK_TRUE;

  // See also Device::make for even simpler factory functions
  const auto device_queue_infos = make_device_queue_infos(mGPU, mSurface);
  const auto device_info = make_device_info(device_queue_infos.queues,
                                            kEnabledLayers,
                                            device_extensions,
                                            &enabled_gpu_features);

  mDevice = Device::make(mGPU, device_info, &result);
  if (!mDevice) {
    std::cerr << "Could not create logical device: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create logical device"};
  }

  mGraphicsQueue = mDevice.get_queue(queue_family_indices.graphics.value());
  mPresentQueue = mDevice.get_queue(queue_family_indices.present.value());
  if (mGraphicsQueue == VK_NULL_HANDLE || mPresentQueue == VK_NULL_HANDLE) {
    throw std::runtime_error {"Missing required device queues"};
  }

  mAllocator = Allocator::make(mInstance, mGPU, mDevice, kTargetVulkanVersion, &result);
  if (!mAllocator) {
    std::cerr << "Could not create allocator: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create allocator"};
  }

  const auto swapchain_support = get_swapchain_support(mGPU, mSurface);
  const auto image_extent =
      pick_image_extent(mWindow, swapchain_support.surface_capabilities);
  mSwapchain = Swapchain::make(mSurface,
                               mGPU,
                               mDevice,
                               mAllocator,
                               image_extent,
                               &_swapchain_surface_format_filter,
                               &_swapchain_present_mode_filter,
                               &result);
  if (!mSwapchain) {
    std::cerr << "Could not create swapchain: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create swapchain"};
  }

  const auto subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  const auto main_subpass_access =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  const auto main_subpass_dependency = make_subpass_dependency(VK_SUBPASS_EXTERNAL,
                                                               0,
                                                               subpass_stages,
                                                               subpass_stages,
                                                               0,
                                                               main_subpass_access);
  mRenderPass = RenderPassBuilder {mDevice}
                    .color_attachment(mSwapchain.info().image_format)
                    // TODO .depth_attachment(swapchain.get_depth_buffer_format())
                    .begin_subpass()
                    .use_color_attachment(0)
                    // TODO .set_depth_attachment(1)
                    .end_subpass()
                    .subpass_dependency(main_subpass_dependency)
                    .build(&result);
  if (!mRenderPass) {
    std::cerr << "Could not create render pass: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create render pass"};
  }

  mPipelineCache = PipelineCache::make(mDevice, nullptr, 0, 0, &result);
  if (!mPipelineCache) {
    std::cerr << "Could not create pipeline cache: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create pipeline cache"};
  }

  mGraphicsCommandPool =
      CommandPool::make(mDevice,
                        queue_family_indices.graphics.value(),
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                        &result);
  if (!mGraphicsCommandPool) {
    std::cerr << "Could not create graphics command pool: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create graphics command pool"};
  }

  mSampler = Sampler::make(mDevice,
                           mGPU,
                           VK_FILTER_LINEAR,
                           VK_SAMPLER_ADDRESS_MODE_REPEAT,
                           &result);
  if (!mSampler) {
    std::cerr << "Could not create sampler: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not create sampler"};
  }

  auto command_buffers = alloc_command_buffers(mDevice,
                                               mGraphicsCommandPool,
                                               static_cast<uint32>(kMaxFramesInFlight),
                                               &result);
  if (result != VK_SUCCESS) {
    std::cerr << "Could not allocate command buffers: " << to_string(result) << '\n';
    throw std::runtime_error {"Could not allocate command buffers"};
  }

  mFrames.reserve(kMaxFramesInFlight);
  for (usize i = 0; i < kMaxFramesInFlight; ++i) {
    auto& frame = mFrames.emplace_back();
    frame.cmd_buffer = command_buffers.at(i);

    frame.in_flight_fence = Fence::make(mDevice, VK_FENCE_CREATE_SIGNALED_BIT, &result);
    if (!frame.in_flight_fence) {
      std::cerr << "Could not create in_flight_fence: " << to_string(result) << '\n';
      throw std::runtime_error {"Could not create in_flight_fence"};
    }

    frame.image_available_semaphore = Semaphore::make(mDevice, 0, &result);
    if (!frame.image_available_semaphore) {
      std::cerr << "Could not create image_available_semaphore: " << to_string(result)
                << '\n';
      throw std::runtime_error {"Could not create image_available_semaphore"};
    }

    frame.render_finished_semaphore = Semaphore::make(mDevice, 0, &result);
    if (!frame.render_finished_semaphore) {
      std::cerr << "Could not create render_finished_semaphore: " << to_string(result)
                << '\n';
      throw std::runtime_error {"Could not create render_finished_semaphore"};
    }
  }

  _recreate_swapchain();
}

void Example::start()
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

    auto& frame = mFrames.at(mFrameIndex);

    // Wait until the GPU has finished executing previously submitted commands.
    frame.in_flight_fence.wait();

    if (_try_acquire_image()) {
      frame.in_flight_fence.reset();

      _render();
      _submit_commands();
      _present_image();

      mFrameIndex = (mFrameIndex + 1) % kMaxFramesInFlight;
    }
  }

  // Wait for the GPU to finish working so that we don't destroy any active resources.
  const auto wait_result = vkDeviceWaitIdle(mDevice);
  if (wait_result != VK_SUCCESS) {
    std::cerr << "vkDeviceWaitIdle failed: " << to_string(wait_result) << '\n';
  }

  mWindow.hide();
}

auto Example::_try_acquire_image() -> bool
{
  auto& frame = mFrames.at(mFrameIndex);

  // Try to acquire an image from the swapchain.
  const auto acquire_result =
      mSwapchain.acquire_next_image(frame.image_available_semaphore);

  // It's fine if the swapchain is suboptimal (then it'll get recreated after the frame).
  return acquire_result == VK_SUCCESS || acquire_result == VK_SUBOPTIMAL_KHR;
}

void Example::_render()
{
  auto& frame = mFrames.at(mFrameIndex);

  const auto cmd_buffer_begin_info = make_command_buffer_begin_info();
  vkResetCommandBuffer(frame.cmd_buffer, 0);
  vkBeginCommandBuffer(frame.cmd_buffer, &cmd_buffer_begin_info);

  std::array<VkClearValue, 1> clear_values = {};
  clear_values[0].color = VkClearColorValue {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}};
  // TODO clear_values[0].depthStencil = VkClearDepthStencilValue {1.0f, 0};

  const auto image_extent = mSwapchain.info().image_extent;
  const auto render_pass_begin_info =
      make_render_pass_begin_info(mRenderPass,
                                  mSwapchain.get_current_framebuffer(),
                                  {VkOffset2D {0, 0}, image_extent},
                                  clear_values.data(),
                                  u32_size(clear_values));
  vkCmdBeginRenderPass(frame.cmd_buffer,
                       &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  const auto viewport = make_viewport(0,
                                      0,
                                      static_cast<float>(image_extent.width),
                                      static_cast<float>(image_extent.height));
  vkCmdSetViewport(frame.cmd_buffer, 0, 1, &viewport);

  const auto scissor = make_rect_2d(0, 0, image_extent.width, image_extent.height);
  vkCmdSetScissor(frame.cmd_buffer, 0, 1, &scissor);

  record_commands();

  vkCmdEndRenderPass(frame.cmd_buffer);
  vkEndCommandBuffer(frame.cmd_buffer);
}

void Example::_submit_commands()
{
  auto& frame = mFrames.at(mFrameIndex);

  // Submit our rendering commands to the graphics queue.
  // 1) Wait on the image_available_semaphore before executing the command buffer.
  // 2) Signal render_finished_semaphore and in_flight_fence after the commands
  //    have finished executing.
  const VkSemaphore wait_semaphores[] = {frame.image_available_semaphore};
  const VkSemaphore signal_semaphores[] = {frame.render_finished_semaphore};
  const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  const auto submit_info = make_submit_info(&frame.cmd_buffer,
                                            1,
                                            wait_semaphores,
                                            1,
                                            &wait_stage,
                                            signal_semaphores,
                                            1);
  const auto submit_result =
      vkQueueSubmit(mGraphicsQueue, 1, &submit_info, frame.in_flight_fence);

  if (submit_result != VK_SUCCESS) {
    std::cerr << "Could not submit commands to graphics queue: "
              << to_string(submit_result) << '\n';
  }
}

void Example::_present_image()
{
  auto& frame = mFrames.at(mFrameIndex);

  const VkSemaphore wait_semaphores[] = {frame.render_finished_semaphore};
  const auto present_result = mSwapchain.present_image(mPresentQueue, wait_semaphores, 1);

  if (present_result == VK_SUCCESS) {
    return;
  }
  else if (present_result == VK_ERROR_OUT_OF_DATE_KHR ||
           present_result == VK_SUBOPTIMAL_KHR) {
    std::cout << "Recreating outdated or suboptimal swapchain\n";
    _recreate_swapchain();
  }
  else {
    std::cerr << "Could not present image: " << to_string(present_result) << '\n';
  }
}

auto Example::_recreate_swapchain() -> VkResult
{
  auto window_size = mWindow.get_size_in_pixels();
  while (window_size.width == 0 || window_size.height == 0) {
    SDL_WaitEvent(nullptr);
    window_size = mWindow.get_size_in_pixels();
  }

  auto& swapchain_info = mSwapchain.info();
  swapchain_info.image_extent = window_size;

  std::cout << "New swapchain image extent: "  //
            << swapchain_info.image_extent.width << 'x'
            << swapchain_info.image_extent.height << '\n';

  return mSwapchain.recreate(mRenderPass, false);  // TODO use depth buffer
}

}  // namespace grace::examples
