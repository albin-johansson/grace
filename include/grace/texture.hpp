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
#include "image.hpp"
#include "image_view.hpp"

namespace grace {

struct Texture final {
  Image image;
  ImageView image_view;

  [[nodiscard]] static auto make_2d(VkDevice device,
                                    VmaAllocator allocator,
                                    const VkExtent2D& extent,
                                    VkImageViewType view_type,
                                    VkFormat format,
                                    VkImageUsageFlags usage,
                                    VkImageAspectFlags view_aspects,
                                    uint32 mip_levels = 1,
                                    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                                    VkResult* result = nullptr) -> Texture;

  void destroy() noexcept;

  [[nodiscard]] explicit operator bool() const noexcept { return image && image_view; }
};

}  // namespace grace
