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

#include "grace/image_view.hpp"

namespace grace {

auto make_image_view_info(VkImage image,
                          const VkImageViewType type,
                          const VkFormat format,
                          const VkImageAspectFlags aspects,
                          const uint32 mip_levels) -> VkImageViewCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .image = image,
      .viewType = type,
      .format = format,
      .components =
          {
              .r = VK_COMPONENT_SWIZZLE_IDENTITY,
              .g = VK_COMPONENT_SWIZZLE_IDENTITY,
              .b = VK_COMPONENT_SWIZZLE_IDENTITY,
              .a = VK_COMPONENT_SWIZZLE_IDENTITY,
          },
      .subresourceRange =
          {
              .aspectMask = aspects,
              .baseMipLevel = 0,
              .levelCount = mip_levels,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };
}

ImageView::ImageView(ImageView&& other) noexcept
    : mDevice {other.mDevice},
      mImageView {other.mImageView}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mImageView = VK_NULL_HANDLE;
}

ImageView& ImageView::operator=(ImageView&& other) noexcept
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mImageView = other.mImageView;

    other.mDevice = VK_NULL_HANDLE;
    other.mImageView = VK_NULL_HANDLE;
  }

  return *this;
}

ImageView::~ImageView() noexcept
{
  destroy();
}

void ImageView::destroy() noexcept
{
  if (mImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(mDevice, mImageView, nullptr);
    mImageView = VK_NULL_HANDLE;
  }
}

auto ImageView::make(VkDevice device, const VkImageViewCreateInfo& info, VkResult* result)
    -> ImageView
{
  ImageView image_view;
  image_view.mDevice = device;

  const auto status = vkCreateImageView(device, &info, nullptr, &image_view.mImageView);

  if (result) {
    *result = status;
  }

  if (status != VK_SUCCESS) {
    return {};
  }

  return image_view;
}

auto ImageView::make(VkDevice device,
                     VkImage image,
                     const VkImageViewType type,
                     const VkFormat format,
                     const VkImageAspectFlags aspects,
                     const uint32 mip_levels,
                     VkResult* result) -> ImageView
{
  const auto info = make_image_view_info(image, type, format, aspects, mip_levels);
  return ImageView::make(device, info, result);
}

}  // namespace grace
