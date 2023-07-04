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

#include "grace/texture.hpp"

namespace grace {

auto Texture::make_2d(VkDevice device,
                      VmaAllocator allocator,
                      const VkExtent2D& extent,
                      const VkImageViewType view_type,
                      const VkFormat format,
                      const VkImageUsageFlags usage,
                      const VkImageAspectFlags view_aspects,
                      const uint32 mip_levels,
                      const VkSampleCountFlagBits samples,
                      VkResult* result) -> Texture
{
  Texture texture;

  texture.image = Image::make(allocator,
                              VK_IMAGE_TYPE_2D,
                              {extent.width, extent.width, 1},
                              format,
                              usage,
                              mip_levels,
                              samples,
                              result);

  if (texture.image) {
    texture.image_view = ImageView::make(device,
                                         texture.image.get(),
                                         view_type,
                                         format,
                                         view_aspects,
                                         mip_levels,
                                         result);
  }

  return texture;
}

void Texture::destroy() noexcept
{
  image_view.destroy();
  image.destroy();
}

}  // namespace grace