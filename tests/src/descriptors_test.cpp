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

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

GRACE_TEST_FIXTURE(DescriptorFixture);

TEST_F(DescriptorFixture, MakeDescriptorBufferInfo)
{
  VkBuffer buffer = make_fake_ptr<VkBuffer>(8'392);
  const VkDeviceSize range = 821;
  const VkDeviceSize offset = 28;

  const auto info = make_descriptor_buffer_info(buffer, range, offset);

  EXPECT_EQ(info.buffer, buffer);
  EXPECT_EQ(info.range, range);
  EXPECT_EQ(info.offset, offset);
}

TEST_F(DescriptorFixture, MakeBufferDescriptorWrite)
{
  VkDescriptorSet set = make_fake_ptr<VkDescriptorSet>(982'120);
  const uint32 binding = 92;
  const auto descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  const uint32 descriptor_count = 1;
  const VkDescriptorBufferInfo buffer_info = {};

  const auto write = make_buffer_descriptor_write(set,
                                                  binding,
                                                  descriptor_type,
                                                  descriptor_count,
                                                  &buffer_info);

  EXPECT_EQ(write.sType, VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
  EXPECT_EQ(write.pNext, nullptr);
  EXPECT_EQ(write.dstSet, set);
  EXPECT_EQ(write.dstBinding, binding);
  EXPECT_EQ(write.descriptorType, descriptor_type);
  EXPECT_EQ(write.descriptorCount, descriptor_count);
  EXPECT_EQ(write.dstArrayElement, 0);
  EXPECT_EQ(write.pBufferInfo, &buffer_info);
  EXPECT_EQ(write.pImageInfo, nullptr);
  EXPECT_EQ(write.pTexelBufferView, nullptr);
}

TEST_F(DescriptorFixture, MakeImageDescriptorWrite)
{
  VkDescriptorSet set = make_fake_ptr<VkDescriptorSet>(732);
  const uint32 binding = 12;
  const auto descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  const uint32 descriptor_count = 1;
  const VkDescriptorImageInfo image_info = {};

  const auto write = make_image_descriptor_write(set,
                                                 binding,
                                                 descriptor_type,
                                                 descriptor_count,
                                                 &image_info);

  EXPECT_EQ(write.sType, VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
  EXPECT_EQ(write.pNext, nullptr);
  EXPECT_EQ(write.dstSet, set);
  EXPECT_EQ(write.dstBinding, binding);
  EXPECT_EQ(write.descriptorType, descriptor_type);
  EXPECT_EQ(write.descriptorCount, descriptor_count);
  EXPECT_EQ(write.dstArrayElement, 0);
  EXPECT_EQ(write.pBufferInfo, nullptr);
  EXPECT_EQ(write.pImageInfo, &image_info);
  EXPECT_EQ(write.pTexelBufferView, nullptr);
}

TEST_F(DescriptorFixture, MakeBufferViewDescriptorWrite)
{
  VkDescriptorSet set = make_fake_ptr<VkDescriptorSet>(732);
  VkBufferView buffer_view = make_fake_ptr<VkBufferView>(912);

  const uint32 binding = 7;
  const auto descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
  const uint32 descriptor_count = 3;

  const auto write = make_buffer_view_descriptor_write(set,
                                                       binding,
                                                       descriptor_type,
                                                       descriptor_count,
                                                       &buffer_view);

  EXPECT_EQ(write.sType, VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
  EXPECT_EQ(write.pNext, nullptr);
  EXPECT_EQ(write.dstSet, set);
  EXPECT_EQ(write.dstBinding, binding);
  EXPECT_EQ(write.descriptorType, descriptor_type);
  EXPECT_EQ(write.descriptorCount, descriptor_count);
  EXPECT_EQ(write.dstArrayElement, 0);
  EXPECT_EQ(write.pBufferInfo, nullptr);
  EXPECT_EQ(write.pImageInfo, nullptr);
  EXPECT_EQ(write.pTexelBufferView, &buffer_view);
}
