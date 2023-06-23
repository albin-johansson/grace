#include "grace/extras/window.hpp"

#ifdef GRACE_USE_SDL2

namespace grace {

void WindowDeleter::operator()(SDL_Window* window) noexcept
{
  SDL_DestroyWindow(window);
}

auto make_window(const char* title, const int width, const int height) -> Window
{
  const auto flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN;
  return Window {SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  width,
                                  height,
                                  flags)};
}

}  // namespace grace

#endif  // GRACE_USE_SDL2
