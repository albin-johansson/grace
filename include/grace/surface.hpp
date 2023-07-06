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

#include <vulkan/vulkan.h>

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL.h>
#endif  // GRACE_USE_SDL2

#include "common.hpp"

namespace grace {

class Surface final {
 public:
#ifdef GRACE_USE_SDL2

  /**
   * Creates a Vulkan surface.
   *
   * \param window   the associated window.
   * \param instance the associated Vulkan instance.
   *
   * \return a potentially null surface.
   */
  [[nodiscard]] static auto make(SDL_Window* window, VkInstance instance) -> Surface;

#endif  // GRACE_USE_SDL2

  Surface() noexcept = default;

  Surface(VkInstance instance, VkSurfaceKHR surface) noexcept;

  Surface(const Surface& other) = delete;
  Surface(Surface&& other) noexcept;

  auto operator=(const Surface& other) -> Surface& = delete;
  auto operator=(Surface&& other) noexcept -> Surface&;

  ~Surface() noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkSurfaceKHR { return mSurface; }

  [[nodiscard]] auto instance() noexcept -> VkInstance { return mInstance; }

  [[nodiscard]] operator VkSurfaceKHR() noexcept { return mSurface; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mSurface != VK_NULL_HANDLE;
  }

 private:
  VkInstance mInstance {VK_NULL_HANDLE};
  VkSurfaceKHR mSurface {VK_NULL_HANDLE};
};

}  // namespace grace
