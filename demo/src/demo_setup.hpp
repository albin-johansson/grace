#pragma once

#include <SDL2/SDL.h>

namespace grace::demo {

class SDL final {
 public:
  SDL();

  ~SDL() noexcept;
};

class Window final {
 public:
  Window();

  ~Window() noexcept;

  [[nodiscard]] auto data() noexcept -> SDL_Window* { return mWindow; }

 private:
  SDL_Window* mWindow;
};

}  // namespace grace::demo
