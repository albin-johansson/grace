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

#include "grace/swapchain.hpp"

#include <algorithm>  // clamp
#include <cassert>    // assert
#include <utility>    // move

#include "grace/physical_device.hpp"
#include "grace/queue.hpp"

namespace grace {

auto make_swapchain_info(VkSurfaceKHR surface,
                         const VkSurfaceCapabilitiesKHR& surface_capabilities,
                         const uint32 min_image_count,
                         const VkExtent2D& image_extent,
                         const VkSurfaceFormatKHR& surface_format,
                         const VkPresentModeKHR& present_mode,
                         const std::vector<uint32>& queue_family_indices,
                         const VkSharingMode image_sharing_mode,
                         VkSwapchainKHR old_swapchain) -> VkSwapchainCreateInfoKHR
{
  return {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .surface = surface,
      .minImageCount = min_image_count,
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = image_extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = image_sharing_mode,
      .queueFamilyIndexCount = u32_size(queue_family_indices),
      .pQueueFamilyIndices = data_or_null(queue_family_indices),
      .preTransform = surface_capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .clipped = VK_TRUE,
      .oldSwapchain = old_swapchain,
  };
}

auto get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
    -> std::vector<VkImage>
{
  uint32 image_count = 0;
  vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);

  std::vector<VkImage> images;
  images.resize(image_count);

  vkGetSwapchainImagesKHR(device, swapchain, &image_count, images.data());

  return images;
}

auto pick_surface_format(const SwapchainSupport& swapchain_support,
                         const SwapchainSurfaceFormatFilter& format_filter)
    -> VkSurfaceFormatKHR
{
  for (const auto surface_format : swapchain_support.surface_formats) {
    if (format_filter(surface_format)) {
      return surface_format;
    }
  }

  return swapchain_support.surface_formats.at(0);
}

auto pick_present_mode(const SwapchainSupport& swapchain_support,
                       const SwapchainPresentModeFilter& mode_filter) -> VkPresentModeKHR
{
  for (const auto mode : swapchain_support.present_modes) {
    if (mode_filter(mode)) {
      return mode;
    }
  }

  // This is the only present mode guaranteed to be supported
  return VK_PRESENT_MODE_FIFO_KHR;
}

#ifdef GRACE_USE_SDL2

auto pick_image_extent(SDL_Window* window, const VkSurfaceCapabilitiesKHR& capabilities)
    -> VkExtent2D
{
  if (capabilities.currentExtent.width != kMaxU32) {
    return capabilities.currentExtent;
  }

  int width = 0;
  int height = 0;
  SDL_GetWindowSizeInPixels(window, &width, &height);

  VkExtent2D extent;

  extent.width = std::clamp(static_cast<uint32>(width),
                            capabilities.minImageExtent.width,
                            capabilities.maxImageExtent.width);

  extent.height = std::clamp(static_cast<uint32>(height),
                             capabilities.minImageExtent.height,
                             capabilities.maxImageExtent.height);

  return extent;
}

#endif  // GRACE_USE_SDL2

void SwapchainInfo::copy_from(const VkSwapchainCreateInfoKHR& swapchain_info)
{
  image_extent = swapchain_info.imageExtent;
  image_format = swapchain_info.imageFormat;
  image_color_space = swapchain_info.imageColorSpace;
  image_sharing_mode = swapchain_info.imageSharingMode;
  present_mode = swapchain_info.presentMode;
  pre_transform = swapchain_info.preTransform;
  min_image_count = swapchain_info.minImageCount;

  if (swapchain_info.queueFamilyIndexCount > 0 && swapchain_info.pQueueFamilyIndices) {
    const auto index_count = static_cast<usize>(swapchain_info.queueFamilyIndexCount);
    queue_family_indices.reserve(index_count);

    for (usize i = 0; i < index_count; ++i) {
      const auto queue_family_index = swapchain_info.pQueueFamilyIndices[i];
      queue_family_indices.push_back(queue_family_index);
    }
  }
}

Swapchain::Swapchain(Swapchain&& other) noexcept
    : mSurface {other.mSurface},
      mDevice {other.mDevice},
      mAllocator {other.mAllocator},
      mSwapchain {other.mSwapchain},
      mInfo {std::move(other.mInfo)},
      mImageIndex {other.mImageIndex},
      mDepthBuffer {std::move(other.mDepthBuffer)},
      mImages {std::move(other.mImages)},
      mImageViews {std::move(other.mImageViews)},
      mFramebuffers {std::move(other.mFramebuffers)}
{
  other.mSurface = VK_NULL_HANDLE;
  other.mDevice = VK_NULL_HANDLE;
  other.mAllocator = VK_NULL_HANDLE;
  other.mSwapchain = VK_NULL_HANDLE;
}

auto Swapchain::operator=(Swapchain&& other) noexcept -> Swapchain&
{
  if (this != &other) {
    destroy();

    mSurface = other.mSurface;
    mDevice = other.mDevice;
    mAllocator = other.mAllocator;
    mSwapchain = other.mSwapchain;
    mInfo = std::move(other.mInfo);
    mImageIndex = other.mImageIndex;
    mDepthBuffer = std::move(other.mDepthBuffer);
    mImages = std::move(other.mImages);
    mImageViews = std::move(other.mImageViews);
    mFramebuffers = std::move(other.mFramebuffers);

    other.mSurface = VK_NULL_HANDLE;
    other.mDevice = VK_NULL_HANDLE;
    other.mAllocator = VK_NULL_HANDLE;
    other.mSwapchain = VK_NULL_HANDLE;
  }

  return *this;
}

Swapchain::~Swapchain() noexcept
{
  destroy();
}

void Swapchain::destroy() noexcept
{
  if (mSwapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
    mSwapchain = VK_NULL_HANDLE;
  }
}

auto Swapchain::make(VkDevice device,
                     VmaAllocator allocator,
                     const VkSwapchainCreateInfoKHR& swapchain_info,
                     VkResult* result) -> Swapchain
{
  VkResult status = VK_ERROR_UNKNOWN;

  Swapchain swapchain;
  status = vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain.mSwapchain);
  if (status != VK_SUCCESS) {
    if (result) {
      *result = status;
    }

    return {};
  }

  swapchain.mSurface = swapchain_info.surface;
  swapchain.mDevice = device;
  swapchain.mAllocator = allocator;
  swapchain.mInfo.copy_from(swapchain_info);
  swapchain.mImageIndex = 0;
  swapchain.mImages = get_swapchain_images(device, swapchain.mSwapchain);

  status = swapchain._recreate_image_views();
  if (status != VK_SUCCESS) {
    if (result) {
      *result = status;
    }

    return {};
  }

  // Note: framebuffers must be created at a later stage by the user.

  if (result) {
    *result = status;
  }

  return swapchain;
}

auto Swapchain::make(VkSurfaceKHR surface,
                     VkPhysicalDevice gpu,
                     VkDevice device,
                     VmaAllocator allocator,
                     const VkExtent2D image_extent,
                     const SwapchainSurfaceFormatFilter& format_filter,
                     const SwapchainPresentModeFilter& present_mode_filter,
                     VkResult* result) -> Swapchain
{
  const auto swapchain_support = get_swapchain_support(gpu, surface);
  const auto surface_format = pick_surface_format(swapchain_support, format_filter);
  const auto present_mode = pick_present_mode(swapchain_support, present_mode_filter);

  uint32 min_image_count = swapchain_support.surface_capabilities.minImageCount + 1;
  min_image_count = std::clamp(min_image_count,
                               swapchain_support.surface_capabilities.minImageCount,
                               swapchain_support.surface_capabilities.maxImageCount);

  const auto queue_family_indices = get_queue_family_indices(gpu, surface);
  const auto unique_queue_family_indices = get_unique_queue_family_indices(gpu, surface);

  const auto sharing_mode =
      (queue_family_indices.graphics != queue_family_indices.present)
          ? VK_SHARING_MODE_CONCURRENT
          : VK_SHARING_MODE_EXCLUSIVE;

  const auto swapchain_info = make_swapchain_info(surface,
                                                  swapchain_support.surface_capabilities,
                                                  min_image_count,
                                                  image_extent,
                                                  surface_format,
                                                  present_mode,
                                                  unique_queue_family_indices,
                                                  sharing_mode);

  return Swapchain::make(device, allocator, swapchain_info, result);
}

auto Swapchain::recreate(VkRenderPass render_pass) -> VkResult
{
  VkResult result = VK_SUCCESS;

  // Avoid touching resources that may still be in use
  result = vkDeviceWaitIdle(mDevice);
  if (result != VK_SUCCESS) {
    return result;
  }

  const VkSwapchainCreateInfoKHR new_swapchain_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .surface = mSurface,
      .minImageCount = mInfo.min_image_count,
      .imageFormat = mInfo.image_format,
      .imageColorSpace = mInfo.image_color_space,
      .imageExtent = mInfo.image_extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = mInfo.image_sharing_mode,
      .queueFamilyIndexCount = u32_size(mInfo.queue_family_indices),
      .pQueueFamilyIndices = data_or_null(mInfo.queue_family_indices),
      .preTransform = mInfo.pre_transform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = mInfo.present_mode,
      .clipped = VK_TRUE,
      .oldSwapchain = mSwapchain,
  };

  auto new_swapchain = Swapchain::make(mDevice, mAllocator, new_swapchain_info, &result);
  if (result != VK_SUCCESS) {
    return result;
  }

  new_swapchain.mInfo.depth_buffer_format = mInfo.depth_buffer_format;
  new_swapchain.mInfo.uses_depth_buffer = mInfo.uses_depth_buffer;

  mDepthBuffer.destroy();
  if (new_swapchain.mInfo.uses_depth_buffer) {
    result = new_swapchain._recreate_depth_buffer();
    if (result != VK_SUCCESS) {
      return result;
    }
  }

  result = new_swapchain._recreate_framebuffers(render_pass);
  if (result != VK_SUCCESS) {
    return result;
  }

  *this = std::move(new_swapchain);
  return result;
}

auto Swapchain::_recreate_image_views() -> VkResult
{
  VkResult result = VK_SUCCESS;

  mImageViews.clear();
  mImageViews.reserve(mImages.size());

  for (VkImage image : mImages) {
    auto image_view = ImageView::make(mDevice,
                                      image,
                                      VK_IMAGE_VIEW_TYPE_2D,
                                      mInfo.image_format,
                                      VK_IMAGE_ASPECT_COLOR_BIT,
                                      1,
                                      &result);
    if (result != VK_SUCCESS) {
      return result;
    }

    mImageViews.push_back(std::move(image_view));
  }

  return result;
}

auto Swapchain::_recreate_framebuffers(VkRenderPass render_pass) -> VkResult
{
  VkResult result = VK_SUCCESS;

  mFramebuffers.clear();
  mFramebuffers.reserve(mImages.size());

  for (auto& image_view : mImageViews) {
    auto framebuffer = Framebuffer::make(mDevice,
                                         render_pass,
                                         image_view.get(),
                                         mDepthBuffer.image_view.get(),
                                         mInfo.image_extent,
                                         &result);

    if (result != VK_SUCCESS) {
      return result;
    }

    mFramebuffers.push_back(std::move(framebuffer));
  }

  return result;
}

auto Swapchain::_recreate_depth_buffer() -> VkResult
{
  VkResult result = VK_SUCCESS;

  const uint32 mip_levels = 1;

  auto image = Image::make(mAllocator,
                           VK_IMAGE_TYPE_2D,
                           {mInfo.image_extent.width, mInfo.image_extent.height, 1},
                           mInfo.depth_buffer_format,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           mip_levels,
                           VK_SAMPLE_COUNT_1_BIT,
                           &result);
  if (result != VK_SUCCESS) {
    return result;
  }

  // TODO aspect flags should vary depending on format (might exclude stencil)
  auto image_view =
      ImageView::make(mDevice,
                      image,
                      VK_IMAGE_VIEW_TYPE_2D,
                      mInfo.depth_buffer_format,
                      VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                      mip_levels,
                      &result);
  if (result != VK_SUCCESS) {
    return result;
  }

  mDepthBuffer.destroy();
  mDepthBuffer.image = std::move(image);
  mDepthBuffer.image_view = std::move(image_view);

  return result;
}

auto Swapchain::acquire_next_image(VkSemaphore semaphore, VkFence fence) -> VkResult
{
  if (!is_ready()) {
    return VK_NOT_READY;
  }

  return vkAcquireNextImageKHR(mDevice,
                               mSwapchain,
                               kMaxU64,
                               semaphore,
                               fence,
                               &mImageIndex);
}

auto Swapchain::present_image(VkQueue queue,
                              const VkSemaphore* wait_semaphores,
                              const uint32 wait_semaphore_count) -> VkResult
{
  const auto present_info = make_present_info(wait_semaphores,
                                              wait_semaphore_count,
                                              &mSwapchain,
                                              1,
                                              &mImageIndex);
  return vkQueuePresentKHR(queue, &present_info);
}

auto Swapchain::get_current_framebuffer() -> VkFramebuffer
{
  const auto current_index = static_cast<usize>(mImageIndex);

  if (is_ready() && current_index < mFramebuffers.size()) {
    return mFramebuffers[current_index].get();
  }

  return VK_NULL_HANDLE;
}

auto Swapchain::get_current_image_index() -> uint32
{
  return mImageIndex;
}

auto Swapchain::get_image_count() const -> uint32
{
  return u32_size(mImages);
}

auto Swapchain::get_depth_buffer_format() const -> VkFormat
{
  if (mDepthBuffer) {
    return mDepthBuffer.image.info().format;
  }

  return VK_FORMAT_UNDEFINED;
}

auto Swapchain::is_ready() const -> bool
{
  return mSwapchain != VK_NULL_HANDLE &&  //
         !mImages.empty() &&              //
         !mImageViews.empty() &&          //
         !mFramebuffers.empty();
}

}  // namespace grace
