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

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

/**
 * Creates an image view creation information structure.
 *
 * \details This function uses the following defaults:\n
 * <ul>
 *   <li>The component mappings are all set to `VK_COMPONENT_SWIZZLE_IDENTITY`.</li>
 *   <li>The base mipmap level is set to 0.</li>
 *   <li>The base array layer is set to 0.</li>
 * </ul>
 *
 * \param image      the associated image.
 * \param type       the image view type.
 * \param format     the format used when interpreting the image texel data.
 * \param aspects    the aspects of the image accessible by the image view.
 * \param mip_levels the number of enabled mipmap levels.
 *
 * \return information required to create an image view.
 */
[[nodiscard]] auto make_image_view_info(
    VkImage image,
    VkImageViewType type,
    VkFormat format,
    VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT,
    uint32 mip_levels = 1) -> VkImageViewCreateInfo;

class ImageView final {
 public:
  ImageView() noexcept = default;

  ImageView(ImageView&& other) noexcept;
  ImageView(const ImageView& other) = delete;

  ImageView& operator=(ImageView&& other) noexcept;
  ImageView& operator=(const ImageView& other) = delete;

  ~ImageView() noexcept;

  /// Destroys the associated Vulkan image view.
  void destroy() noexcept;

  /**
   * Creates a view into an existing image.
   *
   * \param      device the associated logical device.
   * \param      info   the image view information.
   * \param[out] result the resulting error code.
   *
   * \return a potentially null image view.
   */
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkImageViewCreateInfo& info,
                                 VkResult* result = nullptr) -> ImageView;

  /**
   * Creates a view into an existing image.
   *
   * \param      device     the associated logical device.
   * \param      image      the associated image.
   * \param      type       the image view type.
   * \param      format     the image format used when interpreting the image texel data.
   * \param      aspects    the image aspects accessible by the image view.
   * \param      mip_levels the number of mipmap levels.
   * \param[out] result     the resulting error code.
   *
   * \return a potentially null image view.
   */
  [[nodiscard]] static auto make(VkDevice device,
                                 VkImage image,
                                 VkImageViewType type,
                                 VkFormat format,
                                 VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT,
                                 uint32 mip_levels = 1,
                                 VkResult* result = nullptr) -> ImageView;

  [[nodiscard]] auto get() noexcept -> VkImageView { return mImageView; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  /// Indicates whether the image view contains a non-null handle.
  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mImageView != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkImageView mImageView {VK_NULL_HANDLE};
};

}  // namespace grace
