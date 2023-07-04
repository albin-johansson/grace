#pragma once

#include <vulkan/vulkan.h>

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL.h>
#endif  // GRACE_USE_SDL2

#include "common.hpp"

namespace grace {

class Surface final {
 public:
  Surface() noexcept = default;

  Surface(VkInstance instance, VkSurfaceKHR surface) noexcept;

  Surface(const Surface& other) = delete;
  Surface(Surface&& other) noexcept;

  auto operator=(const Surface& other) -> Surface& = delete;
  auto operator=(Surface&& other) noexcept -> Surface&;

  ~Surface() noexcept;

#ifdef GRACE_USE_SDL2

  [[nodiscard]] static auto make(SDL_Window* window, VkInstance instance) -> Surface;

#endif  // GRACE_USE_SDL2

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
