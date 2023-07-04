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

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL_vulkan.h>
#endif  // GRACE_USE_SDL2

namespace grace {

Surface::Surface(VkInstance instance, VkSurfaceKHR surface) noexcept
    : mInstance {instance},
      mSurface {surface}
{
}

Surface::Surface(Surface&& other) noexcept
    : mInstance {other.mInstance},
      mSurface {other.mSurface}
{
  other.mInstance = VK_NULL_HANDLE;
  other.mSurface = VK_NULL_HANDLE;
}

Surface& Surface::operator=(Surface&& other) noexcept
{
  if (this != &other) {
    destroy();

    mInstance = other.mInstance;
    mSurface = other.mSurface;

    other.mInstance = VK_NULL_HANDLE;
    other.mSurface = VK_NULL_HANDLE;
  }

  return *this;
}

Surface::~Surface() noexcept
{
  destroy();
}

void Surface::destroy() noexcept
{
  if (mSurface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    mSurface = VK_NULL_HANDLE;
  }
}

#ifdef GRACE_USE_SDL2

auto Surface::make(SDL_Window* window, VkInstance instance) -> Surface
{
  VkSurfaceKHR surface_handle = VK_NULL_HANDLE;
  if (SDL_Vulkan_CreateSurface(window, instance, &surface_handle) == SDL_TRUE) {
    return Surface {instance, surface_handle};
  }

  return {};
}

#endif  // GRACE_USE_SDL2

}  // namespace grace
