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

#include "grace/allocator.hpp"

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<Allocator, VmaAllocator>);

GRACE_TEST_FIXTURE(AllocatorFixture);

TEST_F(AllocatorFixture, MakeAllocationInfo)
{
  const auto required_mem_props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  const auto preferred_mem_props = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  const auto alloc_flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  const auto memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

  const auto allocation_info = make_allocation_info(required_mem_props,
                                                    preferred_mem_props,
                                                    alloc_flags,
                                                    memory_usage);

  EXPECT_EQ(allocation_info.pool, nullptr);
  EXPECT_EQ(allocation_info.pUserData, nullptr);
  EXPECT_EQ(allocation_info.priority, 0.0f);
  EXPECT_EQ(allocation_info.memoryTypeBits, 0);
  EXPECT_EQ(allocation_info.usage, memory_usage);
  EXPECT_EQ(allocation_info.flags, alloc_flags);
  EXPECT_EQ(allocation_info.requiredFlags, required_mem_props);
  EXPECT_EQ(allocation_info.preferredFlags, preferred_mem_props);
}

TEST_F(AllocatorFixture, Defaults)
{
  Allocator allocator;
  EXPECT_FALSE(allocator);
  EXPECT_EQ(allocator.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VmaAllocator>(allocator), VK_NULL_HANDLE);
  EXPECT_NO_THROW(allocator.destroy());
}

TEST_F(AllocatorFixture, Make)
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto allocator = Allocator::make(mInstance, mGPU, mDevice, {1, 2}, &result);

  EXPECT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(allocator);
}
