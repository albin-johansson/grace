#include "grace/surface.hpp"

#include <stdexcept>  // invalid_argument

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL_vulkan.h>
#endif  // GRACE_USE_SDL2

#include "grace/context.hpp"

namespace grace {

Surface::Surface(VkInstance instance, VkSurfaceKHR surface)
    : mInstance {instance},
      mSurface {surface}
{
  if (!mInstance) {
    throw std::invalid_argument {"Null instance pointer passed to Surface constructor"};
  }

  if (!mSurface) {
    throw std::invalid_argument {"Null surface pointer passed to Surface constructor"};
  }
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
    _destroy();

    mInstance = other.mInstance;
    mSurface = other.mSurface;

    other.mInstance = VK_NULL_HANDLE;
    other.mSurface = VK_NULL_HANDLE;
  }

  return *this;
}

Surface::~Surface() noexcept
{
  _destroy();
}

void Surface::_destroy() noexcept
{
  if (mSurface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  }
}

#ifdef GRACE_USE_SDL2

auto make_surface(SDL_Window* window, VkInstance instance) -> SurfaceResult
{
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  const bool succeeded = SDL_Vulkan_CreateSurface(window, instance, &surface) == SDL_TRUE;

  SurfaceResult result;
  if (succeeded) {
    result.obj.emplace(instance, surface);
  }
  result.status = succeeded ? VK_SUCCESS : VK_ERROR_UNKNOWN;

  return result;
}

#endif  // GRACE_USE_SDL2

}  // namespace grace
