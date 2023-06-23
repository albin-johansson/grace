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

  void show() { SDL_ShowWindow(mWindow); }

  void hide() { SDL_HideWindow(mWindow); }

  [[nodiscard]] auto data() noexcept -> SDL_Window* { return mWindow; }

 private:
  SDL_Window* mWindow;
};

}  // namespace grace::demo
