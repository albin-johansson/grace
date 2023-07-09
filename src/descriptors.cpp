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

#include "grace/descriptors.hpp"

namespace grace {

auto make_descriptor_buffer_info(VkBuffer buffer,
                                 const VkDeviceSize range,
                                 const VkDeviceSize offset) -> VkDescriptorBufferInfo
{
  return {
      .buffer = buffer,
      .offset = offset,
      .range = range,
  };
}

auto make_buffer_descriptor_write(VkDescriptorSet set,
                                  const uint32 binding,
                                  const VkDescriptorType descriptor_type,
                                  const uint32 descriptor_count,
                                  const VkDescriptorBufferInfo* buffer_info)
    -> VkWriteDescriptorSet
{
  return {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = descriptor_count,
      .descriptorType = descriptor_type,
      .pImageInfo = nullptr,
      .pBufferInfo = buffer_info,
      .pTexelBufferView = nullptr,
  };
}

}  // namespace grace
