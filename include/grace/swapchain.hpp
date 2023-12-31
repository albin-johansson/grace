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

#pragma once

#include <functional>  // function
#include <vector>      // vector

#include <vulkan/vulkan.h>

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL.h>
#endif  //  GRACE_USE_SDL2

#include "common.hpp"
#include "framebuffer.hpp"
#include "image_view.hpp"
#include "physical_device.hpp"
#include "texture.hpp"

namespace grace {

[[nodiscard]] auto make_swapchain_info(
    VkSurfaceKHR surface,
    const VkSurfaceCapabilitiesKHR& surface_capabilities,
    uint32 min_image_count,
    const VkExtent2D& image_extent,
    const VkSurfaceFormatKHR& surface_format,
    const VkPresentModeKHR& present_mode,
    const std::vector<uint32>& queue_family_indices,
    VkSharingMode image_sharing_mode,
    VkSwapchainKHR old_swapchain = VK_NULL_HANDLE) -> VkSwapchainCreateInfoKHR;

[[nodiscard]] auto get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
    -> std::vector<VkImage>;

using SwapchainSurfaceFormatFilter = std::function<bool(VkSurfaceFormatKHR)>;
using SwapchainPresentModeFilter = std::function<bool(VkPresentModeKHR)>;

[[nodiscard]] auto pick_surface_format(const SwapchainSupport& swapchain_support,
                                       const SwapchainSurfaceFormatFilter& format_filter)
    -> VkSurfaceFormatKHR;

[[nodiscard]] auto pick_present_mode(const SwapchainSupport& swapchain_support,
                                     const SwapchainPresentModeFilter& mode_filter)
    -> VkPresentModeKHR;

#ifdef GRACE_USE_SDL2

[[nodiscard]] auto pick_image_extent(SDL_Window* window,
                                     const VkSurfaceCapabilitiesKHR& capabilities)
    -> VkExtent2D;

#endif  // GRACE_USE_SDL2

struct SwapchainInfo final {
  VkExtent2D image_extent {0, 0};
  VkFormat image_format {VK_FORMAT_UNDEFINED};
  VkFormat depth_buffer_format {VK_FORMAT_D32_SFLOAT_S8_UINT};
  VkColorSpaceKHR image_color_space {VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  VkSharingMode image_sharing_mode {VK_SHARING_MODE_CONCURRENT};
  VkPresentModeKHR present_mode {VK_PRESENT_MODE_FIFO_KHR};
  VkSurfaceTransformFlagBitsKHR pre_transform {VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR};
  uint32 min_image_count {};
  std::vector<uint32> queue_family_indices;
  bool uses_depth_buffer {false};

  void copy_from(const VkSwapchainCreateInfoKHR& swapchain_info);
};

class Swapchain final {
 public:
  Swapchain() = default;

  Swapchain(Swapchain&& other) noexcept;
  Swapchain(const Swapchain& other) = delete;

  auto operator=(Swapchain&& other) noexcept -> Swapchain&;
  auto operator=(const Swapchain& other) -> Swapchain& = delete;

  ~Swapchain() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 VmaAllocator allocator,
                                 const VkSwapchainCreateInfoKHR& swapchain_info,
                                 VkResult* result = nullptr) -> Swapchain;

  [[nodiscard]] static auto make(VkSurfaceKHR surface,
                                 VkPhysicalDevice gpu,
                                 VkDevice device,
                                 VmaAllocator allocator,
                                 VkExtent2D image_extent,
                                 const SwapchainSurfaceFormatFilter& format_filter,
                                 const SwapchainPresentModeFilter& present_mode_filter,
                                 VkResult* result = nullptr) -> Swapchain;

  void destroy() noexcept;

  auto recreate(VkRenderPass render_pass) -> VkResult;

  auto acquire_next_image(VkSemaphore semaphore = VK_NULL_HANDLE,
                          VkFence fence = VK_NULL_HANDLE) -> VkResult;

  auto present_image(VkQueue queue,
                     const VkSemaphore* wait_semaphores = nullptr,
                     uint32 wait_semaphore_count = 0) -> VkResult;

  [[nodiscard]] auto get_current_framebuffer() -> VkFramebuffer;

  [[nodiscard]] auto get_current_image_index() -> uint32;

  [[nodiscard]] auto get_image_count() const -> uint32;

  [[nodiscard]] auto get_depth_buffer_format() const -> VkFormat;

  [[nodiscard]] auto is_ready() const -> bool;

  [[nodiscard]] auto get() noexcept -> VkSwapchainKHR { return mSwapchain; }
  [[nodiscard]] auto surface() noexcept -> VkSurfaceKHR { return mSurface; }
  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }
  [[nodiscard]] auto allocator() noexcept -> VmaAllocator { return mAllocator; }

  [[nodiscard]] auto info() -> SwapchainInfo& { return mInfo; }
  [[nodiscard]] auto info() const -> const SwapchainInfo& { return mInfo; }

  [[nodiscard]] operator VkSwapchainKHR() noexcept { return mSwapchain; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mSwapchain != VK_NULL_HANDLE;
  }

 private:
  VkSurfaceKHR mSurface {VK_NULL_HANDLE};
  VkDevice mDevice {VK_NULL_HANDLE};
  VmaAllocator mAllocator {VK_NULL_HANDLE};
  VkSwapchainKHR mSwapchain {VK_NULL_HANDLE};
  SwapchainInfo mInfo;
  uint32 mImageIndex {0};
  Texture mDepthBuffer;
  std::vector<VkImage> mImages;
  std::vector<ImageView> mImageViews;
  std::vector<Framebuffer> mFramebuffers;

  auto _recreate_image_views() -> VkResult;
  auto _recreate_framebuffers(VkRenderPass render_pass) -> VkResult;
  auto _recreate_depth_buffer() -> VkResult;
};

}  // namespace grace
