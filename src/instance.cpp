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

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL_vulkan.h>
#endif  // GRACE_USE_SDL2

namespace grace {

void InstanceDeleter::operator()(VkInstance instance) noexcept
{
  vkDestroyInstance(instance, nullptr);
}

auto make_application_info(const char* app_name,
                           const Version& app_version,
                           const ApiVersion& vulkan_version) -> VkApplicationInfo
{
  return VkApplicationInfo {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = app_name,
      .applicationVersion = to_u32(app_version),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
      .apiVersion = to_u32(vulkan_version),
  };
}

auto make_instance_info(const VkApplicationInfo* app_info,
                        const std::vector<const char*>& layers,
                        const std::vector<const char*>& extensions)
    -> VkInstanceCreateInfo
{
  VkInstanceCreateInfo instance_info {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = app_info,
      .enabledLayerCount = u32_size(layers),
      .ppEnabledLayerNames = data_or_null(layers),
      .enabledExtensionCount = u32_size(extensions),
      .ppEnabledExtensionNames = data_or_null(extensions),
  };

#ifdef GRACE_USE_VULKAN_SUBSET
  instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif  // GRACE_USE_VULKAN_SUBSET

  return instance_info;
}

#ifdef GRACE_USE_SDL2

auto get_required_instance_extensions(SDL_Window* window) -> std::vector<const char*>
{
  uint32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

  std::vector<const char*> extensions;
  extensions.resize(extension_count);

  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data());

#ifdef GRACE_USE_VULKAN_SUBSET
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GRACE_USE_VULKAN_SUBSET

  return extensions;
}

#endif  // GRACE_USE_SDL2

auto Instance::make(const char* app_name,
                    const std::vector<const char*>& layers,
                    const std::vector<const char*>& extensions,
                    const Version& app_version,
                    const ApiVersion& vulkan_version,
                    VkResult* result) -> Instance
{
  const auto app_info = make_application_info(app_name, app_version, vulkan_version);
  const auto instance_info = make_instance_info(&app_info, layers, extensions);

  VkInstance instance_handle = VK_NULL_HANDLE;
  const auto status = vkCreateInstance(&instance_info, nullptr, &instance_handle);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    Instance instance;
    instance.mInstance.reset(instance_handle);
    return instance;
  }

  return {};
}

}  // namespace grace
