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

#include "grace/buffer.hpp"

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<Buffer, VkBuffer>);

TEST(Buffers, MakeBufferInfo)
{
  const auto buffer_info = make_buffer_info(1'000, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  EXPECT_EQ(buffer_info.sType, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
  EXPECT_EQ(buffer_info.size, 1'000);
  EXPECT_EQ(buffer_info.usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  EXPECT_EQ(buffer_info.pNext, nullptr);
  EXPECT_EQ(buffer_info.flags, 0);
  EXPECT_EQ(buffer_info.pQueueFamilyIndices, nullptr);
  EXPECT_EQ(buffer_info.sharingMode, VK_SHARING_MODE_EXCLUSIVE);
}
