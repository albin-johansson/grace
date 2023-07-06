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

#include <doctest/doctest.h>
#include <vulkan/vulkan.h>

#include "grace/extras/window.hpp"

TEST_SUITE("Instance")
{
  TEST_CASE("make_application_info")
  {
    const auto app_info = grace::make_application_info("Tests", {1, 2, 3}, {1, 2});

    CHECK(app_info.sType == VK_STRUCTURE_TYPE_APPLICATION_INFO);
    CHECK(app_info.pNext == nullptr);
    CHECK(app_info.pApplicationName == doctest::String("Tests"));
    CHECK(app_info.pEngineName == doctest::String("No Engine"));
    CHECK(app_info.apiVersion == VK_API_VERSION_1_2);
    CHECK(app_info.applicationVersion == VK_MAKE_VERSION(1, 2, 3));
    CHECK(app_info.engineVersion == VK_MAKE_VERSION(0, 1, 0));
  }

  TEST_CASE("make_instance_info")
  {
    const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const auto app_info = grace::make_application_info("Tests", {1, 2, 3}, {1, 2});
    const auto instance_info = grace::make_instance_info(&app_info, layers, extensions);

    CHECK(instance_info.sType == VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
    CHECK(instance_info.pNext == nullptr);
    CHECK(instance_info.pApplicationInfo == &app_info);
    CHECK(instance_info.enabledLayerCount == 1);
    CHECK(instance_info.ppEnabledLayerNames == layers.data());
    CHECK(instance_info.enabledExtensionCount == 1);
    CHECK(instance_info.ppEnabledExtensionNames == extensions.data());

#ifdef GRACE_USE_VULKAN_SUBSET
    CHECK(instance_info.flags == VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR);
#else
    CHECK(instance_info.flags == 0);
#endif  // GRACE_USE_VULKAN_SUBSET
  }

  TEST_CASE("make_instance")
  {
    const std::vector<const char*> layers;
    std::vector<const char*> extensions;

#ifdef GRACE_USE_VULKAN_SUBSET
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GRACE_USE_VULKAN_SUBSET

    VkResult result = VK_SUCCESS;
    auto instance =
        grace::Instance::make("Tests", layers, extensions, {0, 1, 0}, {1, 2}, &result);

    CHECK(instance != nullptr);
    CHECK(result == VK_SUCCESS);
  }

#ifdef GRACE_USE_SDL2

  TEST_CASE("make_instance with get_required_instance_extensions")
  {
    auto window = grace::Window::make("Test", 800, 600);

    const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
    const auto extensions = grace::get_required_instance_extensions(window);

    VkResult result = VK_SUCCESS;
    auto instance =
        grace::Instance::make("Tests", layers, extensions, {0, 1, 0}, {1, 0}, &result);

    CHECK(instance != nullptr);
    CHECK(result == VK_SUCCESS);
  }

#endif  // GRACE_USE_SDL2
}
