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

#include "grace/semaphore.hpp"

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<Semaphore, VkSemaphore>);

GRACE_TEST_FIXTURE(SemaphoreFixture);

TEST_F(SemaphoreFixture, MakeSemaphoreInfo)
{
  const auto semaphore_info = make_semaphore_info();

  EXPECT_EQ(semaphore_info.sType, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO);
  EXPECT_EQ(semaphore_info.pNext, nullptr);
  EXPECT_EQ(semaphore_info.flags, 0);
}

TEST_F(SemaphoreFixture, Defaults)
{
  Semaphore semaphore;
  EXPECT_FALSE(semaphore);
  EXPECT_EQ(semaphore.device(), VK_NULL_HANDLE);
  EXPECT_EQ(semaphore.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkSemaphore>(semaphore), VK_NULL_HANDLE);
  EXPECT_NO_THROW(semaphore.destroy());
}

TEST_F(SemaphoreFixture, Make)
{
  auto semaphore = Semaphore::make(mDevice);

  EXPECT_TRUE(semaphore);
  EXPECT_EQ(semaphore.device(), mDevice);
  EXPECT_NE(semaphore.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkSemaphore>(semaphore), semaphore.get());

  semaphore.destroy();
  EXPECT_FALSE(semaphore);
  EXPECT_EQ(semaphore.device(), mDevice);
  EXPECT_EQ(semaphore.get(), VK_NULL_HANDLE);
}
