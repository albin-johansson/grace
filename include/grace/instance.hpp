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

#pragma once

#include <memory>  // unique_ptr
#include <vector>  // vector

#include <vulkan/vulkan.h>

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL.h>
#endif  // GRACE_USE_SDL2

#include "grace/common.hpp"
#include "grace/version.hpp"

namespace grace {

struct InstanceDeleter final {
  void operator()(VkInstance instance) noexcept;
};

using UniqueInstance = std::unique_ptr<VkInstance_T, InstanceDeleter>;

[[nodiscard]] auto make_application_info(const char* app_name,
                                         const Version& app_version,
                                         const ApiVersion& vulkan_version)
    -> VkApplicationInfo;

[[nodiscard]] auto make_instance_info(const VkApplicationInfo* app_info,
                                      const std::vector<const char*>& layers,
                                      const std::vector<const char*>& extensions)
    -> VkInstanceCreateInfo;

#ifdef GRACE_USE_SDL2

/**
 * Returns the names of required instance extensions.
 *
 * \param window the target Vulkan window.
 *
 * \return a collection of Vulkan extensions names.
 */
[[nodiscard]] auto get_required_instance_extensions(SDL_Window* window)
    -> std::vector<const char*>;

#endif  // GRACE_USE_SDL2

class Instance final {
 public:
  /**
   * Creates a Vulkan instance.
   *
   * \details The following is a simple example on how to use this function.
   *
   * \code{cpp}
   *   const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
   *   const auto extensions = grace::get_required_instance_extensions();
   *   if (auto instance = grace::Instance::make("Vulkan App", layers, extensions)) {
   *     // Instance created successfully
   *   }
   * \endcode
   *
   * \param      app_name       the application name.
   * \param      layers         the names of required layers.
   * \param      extensions     the names of required extensions.
   * \param      app_version    optional application version.
   * \param      vulkan_version the target Vulkan API version.
   * \param[out] result         the resulting error code.
   *
   * \return a Vulkan instance on success; a null pointer on failure.
   */
  [[nodiscard]] static auto make(const char* app_name,
                                 const std::vector<const char*>& layers,
                                 const std::vector<const char*>& extensions,
                                 const Version& app_version = {0, 1, 0},
                                 const ApiVersion& vulkan_version = {1, 2},
                                 VkResult* result = nullptr) -> Instance;

  [[nodiscard]] auto get() noexcept -> VkInstance { return mInstance.get(); }

  [[nodiscard]] operator VkInstance() noexcept { return mInstance.get(); }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mInstance != VK_NULL_HANDLE;
  }

 private:
  std::unique_ptr<VkInstance_T, InstanceDeleter> mInstance;
};

}  // namespace grace
