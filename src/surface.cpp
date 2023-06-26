#include "grace/surface.hpp"

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL_vulkan.h>
#endif  // GRACE_USE_SDL2

namespace grace {
namespace {

void _destroy_surface(VkInstance instance, VkSurfaceKHR surface) noexcept
{
  if (instance != VK_NULL_HANDLE && surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }
}

}  // namespace

Surface::Surface(Surface&& other) noexcept
    : instance {other.instance},
      ptr {other.ptr}
{
  other.instance = VK_NULL_HANDLE;
  other.ptr = VK_NULL_HANDLE;
}

Surface& Surface::operator=(Surface&& other) noexcept
{
  if (this != &other) {
    _destroy_surface(instance, ptr);

    instance = other.instance;
    ptr = other.ptr;

    other.instance = VK_NULL_HANDLE;
    other.ptr = VK_NULL_HANDLE;
  }

  return *this;
}

Surface::~Surface() noexcept
{
  _destroy_surface(instance, ptr);
}

#ifdef GRACE_USE_SDL2

auto make_surface(SDL_Window* window, VkInstance instance) -> std::optional<Surface>
{
  Surface surface;
  surface.instance = instance;

  if (SDL_Vulkan_CreateSurface(window, instance, &surface.ptr) == SDL_TRUE) {
    return surface;
  }

  return std::nullopt;
}

#endif  // GRACE_USE_SDL2

}  // namespace grace
