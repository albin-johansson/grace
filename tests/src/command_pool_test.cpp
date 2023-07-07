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

#include "grace/command_pool.hpp"

#include <gtest/gtest.h>

#include "grace/physical_device.hpp"
#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<CommandPool, VkCommandPool>);

GRACE_TEST_FIXTURE(CommandPoolFixture);

TEST_F(CommandPoolFixture, MakeCommandPoolInfo)
{
  const uint32 queue_family_index = 123;
  const auto flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  const auto info = make_command_pool_info(queue_family_index, flags);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.queueFamilyIndex, queue_family_index);
  EXPECT_EQ(info.flags, flags);
}

TEST_F(CommandPoolFixture, MakeCommandBufferAllocInfo)
{
  auto* cmd_pool = make_fake_ptr<VkCommandPool>(82);
  const uint32 command_buffer_count = 7;

  const auto info = make_command_buffer_alloc_info(cmd_pool, command_buffer_count);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.level, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  EXPECT_EQ(info.commandPool, cmd_pool);
  EXPECT_EQ(info.commandBufferCount, command_buffer_count);
}

TEST_F(CommandPoolFixture, Defaults)
{
  CommandPool command_pool;
  EXPECT_FALSE(command_pool);
  EXPECT_EQ(command_pool.device(), VK_NULL_HANDLE);
  EXPECT_EQ(command_pool.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkCommandPool>(command_pool), VK_NULL_HANDLE);
  EXPECT_NO_THROW(command_pool.destroy());
}

TEST_F(CommandPoolFixture, Make)
{
  const auto queue_family_indices = get_queue_family_indices(mGPU, mSurface);

  VkResult result = VK_ERROR_UNKNOWN;
  auto command_pool =
      CommandPool::make(mDevice, queue_family_indices.graphics.value(), 0, &result);

  ASSERT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(command_pool);
  EXPECT_EQ(command_pool.device(), mDevice);
  EXPECT_NE(command_pool.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkCommandPool>(command_pool), command_pool.get());

  command_pool.destroy();
  EXPECT_FALSE(command_pool);
  EXPECT_EQ(command_pool.device(), mDevice);
  EXPECT_EQ(command_pool.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkCommandPool>(command_pool), VK_NULL_HANDLE);
}
