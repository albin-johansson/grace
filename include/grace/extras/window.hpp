#pragma once

#ifdef GRACE_USE_SDL2

#include <memory>  // unique_ptr

#include <SDL2/SDL.h>

namespace grace {

struct WindowDeleter final {
  void operator()(SDL_Window* window) noexcept;
};

class Window final {
 public:
  Window() noexcept = default;

  explicit Window(SDL_Window* window) noexcept;

  [[nodiscard]] static auto make(const char* title, int width, int height) -> Window;

  [[nodiscard]] auto get() noexcept -> SDL_Window* { return mWindow.get(); }

  [[nodiscard]] operator SDL_Window*() noexcept { return mWindow.get(); }

  [[nodiscard]] explicit operator bool() const noexcept { return mWindow != nullptr; }

 private:
  std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
};

}  // namespace grace

#endif  // GRACE_USE_SDL2