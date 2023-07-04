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
