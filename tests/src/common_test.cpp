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

#include "grace/common.hpp"

#include <array>   // array
#include <vector>  // vector

#include <gtest/gtest.h>

using namespace grace;

static_assert(sizeof(int32) == 4);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(uint64) == 8);

TEST(Common, VkResultToString)
{
  EXPECT_STREQ(to_string(VK_SUCCESS), "SUCCESS");
  EXPECT_STREQ(to_string(VK_NOT_READY), "NOT_READY");
  EXPECT_STREQ(to_string(VK_TIMEOUT), "TIMEOUT");
  EXPECT_STREQ(to_string(VK_EVENT_SET), "EVENT_SET");
  EXPECT_STREQ(to_string(VK_EVENT_RESET), "EVENT_RESET");
  EXPECT_STREQ(to_string(VK_INCOMPLETE), "INCOMPLETE");
  EXPECT_STREQ(to_string(VK_ERROR_OUT_OF_HOST_MEMORY), "ERROR_OUT_OF_HOST_MEMORY");
  EXPECT_STREQ(to_string(VK_ERROR_OUT_OF_DEVICE_MEMORY), "ERROR_OUT_OF_DEVICE_MEMORY");
  EXPECT_STREQ(to_string(VK_ERROR_INITIALIZATION_FAILED), "ERROR_INITIALIZATION_FAILED");
  EXPECT_STREQ(to_string(VK_ERROR_DEVICE_LOST), "ERROR_DEVICE_LOST");
  EXPECT_STREQ(to_string(VK_ERROR_MEMORY_MAP_FAILED), "ERROR_MEMORY_MAP_FAILED");
  EXPECT_STREQ(to_string(VK_ERROR_LAYER_NOT_PRESENT), "ERROR_LAYER_NOT_PRESENT");
  EXPECT_STREQ(to_string(VK_ERROR_EXTENSION_NOT_PRESENT), "ERROR_EXTENSION_NOT_PRESENT");
  EXPECT_STREQ(to_string(VK_ERROR_FEATURE_NOT_PRESENT), "ERROR_FEATURE_NOT_PRESENT");
  EXPECT_STREQ(to_string(VK_ERROR_INCOMPATIBLE_DRIVER), "ERROR_INCOMPATIBLE_DRIVER");
  EXPECT_STREQ(to_string(VK_ERROR_TOO_MANY_OBJECTS), "ERROR_TOO_MANY_OBJECTS");
  EXPECT_STREQ(to_string(VK_ERROR_FORMAT_NOT_SUPPORTED), "ERROR_FORMAT_NOT_SUPPORTED");
  EXPECT_STREQ(to_string(VK_ERROR_FRAGMENTED_POOL), "ERROR_FRAGMENTED_POOL");
  EXPECT_STREQ(to_string(VK_ERROR_UNKNOWN), "ERROR_UNKNOWN");
  EXPECT_STREQ(to_string(VK_ERROR_OUT_OF_POOL_MEMORY), "ERROR_OUT_OF_POOL_MEMORY");
  EXPECT_STREQ(to_string(VK_ERROR_INVALID_EXTERNAL_HANDLE),
               "ERROR_INVALID_EXTERNAL_HANDLE");
  EXPECT_STREQ(to_string(VK_ERROR_FRAGMENTATION), "ERROR_FRAGMENTATION");
  EXPECT_STREQ(to_string(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS),
               "ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS");
  EXPECT_STREQ(to_string(VK_PIPELINE_COMPILE_REQUIRED), "PIPELINE_COMPILE_REQUIRED");

  // These are from the swapchain extension which should always be available.
  EXPECT_STREQ(to_string(VK_SUBOPTIMAL_KHR), "SUBOPTIMAL_KHR");
  EXPECT_STREQ(to_string(VK_ERROR_OUT_OF_DATE_KHR), "ERROR_OUT_OF_DATE_KHR");
}

TEST(Common, U32Size)
{
  const std::vector vec = {1, 2, 3, 4, 5};
  const std::array arr = {1, 2, 3, 4, 5};
  EXPECT_EQ(u32_size(vec), static_cast<uint32>(vec.size()));
  EXPECT_EQ(u32_size(arr), static_cast<uint32>(arr.size()));
}

TEST(Common, DataOrNull)
{
  {
    const std::vector<int> vec;
    const std::vector<int> arr;
    EXPECT_EQ(data_or_null(vec), nullptr);
    EXPECT_EQ(data_or_null(arr), nullptr);
  }

  {
    const std::vector vec = {"A"};
    const std::array arr = {"A"};
    EXPECT_EQ(data_or_null(vec), vec.data());
    EXPECT_EQ(data_or_null(arr), arr.data());
  }
}
