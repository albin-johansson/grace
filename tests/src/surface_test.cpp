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

#include "grace/surface.hpp"

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<Surface, VkSurfaceKHR>);

GRACE_TEST_FIXTURE(SurfaceFixture);

TEST_F(SurfaceFixture, Defaults)
{
  Surface surface;
  EXPECT_FALSE(surface);
  EXPECT_EQ(surface.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkSurfaceKHR>(surface), VK_NULL_HANDLE);
}

TEST_F(SurfaceFixture, Make)
{
  auto surface = Surface::make(mWindow, mInstance);
  EXPECT_TRUE(surface);
  EXPECT_NE(surface.get(), VK_NULL_HANDLE);
  EXPECT_NE(static_cast<VkSurfaceKHR>(surface), VK_NULL_HANDLE);
  EXPECT_NO_THROW(surface.destroy());
}
