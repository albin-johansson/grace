#pragma once

#ifdef GRACE_USE_SDL2

#include <memory>  // unique_ptr

#include <SDL2/SDL.h>

namespace grace {

struct WindowDeleter final {
  void operator()(SDL_Window* window) noexcept;
};

using Window = std::unique_ptr<SDL_Window, WindowDeleter>;

[[nodiscard]] auto make_window(const char* title, int width, int height) -> Window;

}  // namespace grace

#endif  // GRACE_USE_SDL2