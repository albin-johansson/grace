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

#include "grace/physical_device.hpp"

#include <algorithm>  // count

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

GRACE_TEST_FIXTURE(PhysicalDeviceFixture);

TEST_F(PhysicalDeviceFixture, GetPhysicalDevices)
{
  const auto gpus = get_physical_devices(mInstance);
  EXPECT_FALSE(gpus.empty());
}

TEST_F(PhysicalDeviceFixture, GetExtensions)
{
  const auto extensions = get_extensions(mGPU);
  EXPECT_FALSE(extensions.empty());
}

TEST_F(PhysicalDeviceFixture, GetQueueFamilies)
{
  const auto queue_families = get_queue_families(mGPU);
  EXPECT_FALSE(queue_families.empty());
}

TEST_F(PhysicalDeviceFixture, GetSurfaceFormats)
{
  const auto surface_formats = get_surface_formats(mGPU, mSurface);
  EXPECT_FALSE(surface_formats.empty());
}

TEST_F(PhysicalDeviceFixture, GetPresentModes)
{
  const auto present_modes = get_present_modes(mGPU, mSurface);
  EXPECT_FALSE(present_modes.empty());
}

TEST_F(PhysicalDeviceFixture, GetQueueFamilyIndices)
{
  const auto queue_family_indices = get_queue_family_indices(mGPU, mSurface);
  EXPECT_TRUE(queue_family_indices.graphics.has_value());
  EXPECT_TRUE(queue_family_indices.present.has_value());
}

TEST_F(PhysicalDeviceFixture, GetUniqueQueueFamilyIndices)
{
  const auto queue_family_indices = get_unique_queue_family_indices(mGPU, mSurface);

  for (const auto queue_family_index : queue_family_indices) {
    EXPECT_EQ(std::count(queue_family_indices.begin(),
                         queue_family_indices.end(),
                         queue_family_index),
              1);
  }
}

TEST_F(PhysicalDeviceFixture, GetSwapchainSupport)
{
  const auto support = get_swapchain_support(mGPU, mSurface);
  EXPECT_FALSE(support.surface_formats.empty());
  EXPECT_FALSE(support.present_modes.empty());
}

TEST_F(PhysicalDeviceFixture, PickPhysicalDevice)
{
  auto true_filter = [](VkPhysicalDevice, VkSurfaceKHR) { return true; };
  auto false_filter = [](VkPhysicalDevice, VkSurfaceKHR) { return false; };
  auto rater = [](VkPhysicalDevice) { return 0; };

  EXPECT_NE(pick_physical_device(mInstance, mSurface, true_filter, rater),
            VK_NULL_HANDLE);
  EXPECT_EQ(pick_physical_device(mInstance, mSurface, false_filter, rater),
            VK_NULL_HANDLE);
}