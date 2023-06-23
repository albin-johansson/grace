#pragma once

#include <memory>  // unique_ptr

#include <vulkan/vulkan.h>

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL.h>
#endif  // GRACE_USE_SDL2

#include "grace/common.hpp"

namespace grace {

class Surface final {
 public:
  Surface(VkInstance instance, VkSurfaceKHR surface);

  Surface(Surface&& other) noexcept;
  Surface& operator=(Surface&& other) noexcept;

  Surface(const Surface&) = delete;
  Surface& operator=(const Surface&) = delete;

  ~Surface() noexcept;

  [[nodiscard]] auto ptr() noexcept -> VkSurfaceKHR { return mSurface; }

  [[nodiscard]] auto instance() noexcept -> VkInstance { return mInstance; }

 private:
  VkInstance mInstance;
  VkSurfaceKHR mSurface;

  void _destroy() noexcept;
};

#ifdef GRACE_USE_SDL2

using SurfaceResult = Result<Surface>;

[[nodiscard]] auto make_surface(SDL_Window* window, VkInstance instance) -> SurfaceResult;

#endif  // GRACE_USE_SDL2

}  // namespace grace
