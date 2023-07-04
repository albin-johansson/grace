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

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common.hpp"
#include "context.hpp"

namespace grace {

/**
 * Creates an image creation information structure.
 *
 * \details This function uses the following defaults:\n
 * <ul>
 *   <li>The tiling mode is set to `VK_IMAGE_TILING_OPTIMAL`.</li>
 *   <li>The sharing mode is set to `VK_SHARING_MODE_EXCLUSIVE`.</li>
 *   <li>The initial layout is set to `VK_IMAGE_LAYOUT_UNDEFINED`.</li>
 * </ul>
 *
 * \param type       the image type.
 * \param extent     the image dimensions (use depth of 1 for 2D images).
 * \param format     the texel data format.
 * \param usage      the image usage hint flags. The `VK_IMAGE_USAGE_TRANSFER_SRC_BIT` and
 *                   `VK_IMAGE_USAGE_TRANSFER_DST_BIT` flags are automatically included.
 * \param mip_levels the number of supported mipmap levels (ignored and set to 1 if
 *                   supersampling is used).
 * \param samples    the number of samples per texel.
 *
 * \return information required to create an image.
 */
[[nodiscard]] auto make_image_info(VkImageType type,
                                   const VkExtent3D& extent,
                                   VkFormat format,
                                   VkImageUsageFlags usage,
                                   uint32 mip_levels,
                                   VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT)
    -> VkImageCreateInfo;

[[nodiscard]] auto get_max_image_mip_levels(const VkExtent3D& extent) -> uint32;

void cmd_change_image_layout(VkCommandBuffer cmd_buf,
                             VkImage image,
                             VkImageLayout old_layout,
                             VkImageLayout new_layout,
                             uint32 base_mip_level,
                             uint32 mip_level_count);

void cmd_copy_buffer_to_image(VkCommandBuffer cmd_buf,
                              VkBuffer buffer,
                              VkImage image,
                              const VkExtent3D& image_extent,
                              VkImageLayout image_layout);

struct ImageInfo final {
  VkExtent3D extent {0, 0, 0};
  VkImageLayout layout {VK_IMAGE_LAYOUT_UNDEFINED};
  VkFormat format {VK_FORMAT_UNDEFINED};
  VkSampleCountFlagBits samples {VK_SAMPLE_COUNT_1_BIT};
  uint32 mip_levels {1};

  void copy_from(const VkImageCreateInfo& image_info);
};

class Image final {
 public:
  Image() noexcept = default;

  Image(Image&& other) noexcept;
  Image(const Image& other) = delete;

  Image& operator=(Image&& other) noexcept;
  Image& operator=(const Image& other) = delete;

  ~Image() noexcept;

  /// Destroys the associated Vulkan image.
  void destroy() noexcept;

  /**
   * Creates an image.
   *
   * \param      allocator       the associated memory allocator.
   * \param      image_info      the image specification.
   * \param      allocation_info the allocation specification.
   * \param[out] result          the resulting error code.
   *
   * \return a potentially null image.
   */
  [[nodiscard]] static auto make(VmaAllocator allocator,
                                 const VkImageCreateInfo& image_info,
                                 const VmaAllocationCreateInfo& allocation_info,
                                 VkResult* result = nullptr) -> Image;

  /**
   * Creates an image.
   *
   * \param      allocator  the associated memory allocator.
   * \param      type       the image type.
   * \param      extent     the image dimensions.
   * \param      format     the texel data format.
   * \param      usage      the image usage hint flags.
   * \param      mip_levels the number of mipmap levels.
   * \param      samples    the number of samples per texel.
   * \param[out] result     the resulting error code.
   *
   * \return a potentially null image.
   */
  [[nodiscard]] static auto make(VmaAllocator allocator,
                                 VkImageType type,
                                 const VkExtent3D& extent,
                                 VkFormat format,
                                 VkImageUsageFlags usage,
                                 uint32 mip_levels,
                                 VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                                 VkResult* result = nullptr) -> Image;

  auto set_data(const CommandContext& ctx,
                VmaAllocator allocator,
                const void* data,
                uint64 data_size) -> VkResult;

  void change_layout(const CommandContext& ctx, VkImageLayout new_layout);

  void copy_buffer(const CommandContext& ctx, VkBuffer buffer);

  void generate_mipmaps(const CommandContext& ctx);

  [[nodiscard]] auto get() noexcept -> VkImage { return mImage; }

  [[nodiscard]] auto allocator() noexcept -> VmaAllocator { return mAllocator; }

  [[nodiscard]] auto allocation() noexcept -> VmaAllocation { return mAllocation; }

  [[nodiscard]] auto info() noexcept -> ImageInfo& { return mInfo; }
  [[nodiscard]] auto info() const noexcept -> const ImageInfo& { return mInfo; }

  /// Indicates whether the image contains a non-null handle.
  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mImage != VK_NULL_HANDLE;
  }

 private:
  VmaAllocator mAllocator {VK_NULL_HANDLE};
  VkImage mImage {VK_NULL_HANDLE};
  VmaAllocation mAllocation {VK_NULL_HANDLE};
  ImageInfo mInfo;
};

}  // namespace grace
