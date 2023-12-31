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

#include "grace/instance.hpp"

#include <gtest/gtest.h>

#include "grace/extras/window.hpp"
#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<Instance, VkInstance>);

TEST(Instance, MakeApplicationInfo)
{
  const char* app_name = "Tests";
  const Version app_version = {1, 2, 3};
  const ApiVersion api_version = {1, 2};

  const auto app_info = make_application_info(app_name, app_version, api_version);

  EXPECT_EQ(app_info.sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);
  EXPECT_EQ(app_info.pNext, nullptr);
  EXPECT_EQ(app_info.apiVersion, to_u32(api_version));
  EXPECT_EQ(app_info.applicationVersion, to_u32(app_version));
  EXPECT_EQ(app_info.engineVersion, VK_MAKE_VERSION(0, 1, 0));
  EXPECT_STREQ(app_info.pApplicationName, app_name);
  EXPECT_STREQ(app_info.pEngineName, "No Engine");
}

TEST(Instance, MakeInstanceInfo)
{
  const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector extensions = {VK_KHR_SURFACE_EXTENSION_NAME};

  const auto app_info = make_application_info("Tests", {1, 2, 3}, {1, 2});
  const auto instance_info = make_instance_info(&app_info, layers, extensions);

  EXPECT_EQ(instance_info.sType, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
  EXPECT_EQ(instance_info.pNext, nullptr);
  EXPECT_EQ(instance_info.pApplicationInfo, &app_info);
  EXPECT_EQ(instance_info.enabledLayerCount, u32_size(layers));
  EXPECT_EQ(instance_info.ppEnabledLayerNames, layers.data());
  EXPECT_EQ(instance_info.enabledExtensionCount, u32_size(extensions));
  EXPECT_EQ(instance_info.ppEnabledExtensionNames, extensions.data());

#ifdef GRACE_USE_VULKAN_SUBSET
  EXPECT_EQ(instance_info.flags, VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR);
#else
  EXPECT_EQ(instance_info.flags, 0);
#endif  // GRACE_USE_VULKAN_SUBSET
}

TEST(Instance, Defaults)
{
  Instance instance;
  EXPECT_FALSE(instance);
  EXPECT_EQ(instance.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkInstance>(instance), VK_NULL_HANDLE);
}

TEST(Instance, MakeInstance)
{
  const std::vector<const char*> layers;
  std::vector<const char*> extensions;

#ifdef GRACE_USE_VULKAN_SUBSET
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GRACE_USE_VULKAN_SUBSET

  VkResult result = VK_ERROR_UNKNOWN;
  auto instance = Instance::make("Tests", layers, extensions, {0, 1, 0}, {1, 2}, &result);

  EXPECT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(instance);
}

TEST(Instance, MakeInstanceWithGetRequiredInstanceExtensions)
{
  auto window = Window::make("Test", 800, 600);

  const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
  const auto extensions = get_required_instance_extensions(window);

  VkResult result = VK_ERROR_UNKNOWN;
  auto instance = Instance::make("Tests", layers, extensions, {0, 1, 0}, {1, 0}, &result);

  EXPECT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(instance);
}
