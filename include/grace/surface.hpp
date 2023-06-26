#pragma once

#include <memory>    // unique_ptr
#include <optional>  // optional

#include <vulkan/vulkan.h>

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL.h>
#endif  // GRACE_USE_SDL2

#include "grace/common.hpp"

namespace grace {

struct Surface final {
  Surface() = default;
  ~Surface() noexcept;

  Surface(const Surface& other) = delete;
  Surface(Surface&& other) noexcept;

  Surface& operator=(const Surface& other) = delete;
  Surface& operator=(Surface&& other) noexcept;

  VkInstance instance {VK_NULL_HANDLE};
  VkSurfaceKHR ptr {VK_NULL_HANDLE};
};

#ifdef GRACE_USE_SDL2

[[nodiscard]] auto make_surface(SDL_Window* window, VkInstance instance)
    -> std::optional<Surface>;

#endif  // GRACE_USE_SDL2

}  // namespace grace
