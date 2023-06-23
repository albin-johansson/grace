#include "demo_setup.hpp"

#include <stdexcept>  // runtime_error

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace grace::demo {

SDL::SDL()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw std::runtime_error {"Could not initialize SDL"};
  }

  if (SDL_Vulkan_LoadLibrary(nullptr) == -1) {
    SDL_Quit();
    throw std::runtime_error {"Could not load Vulkan library"};
  }
}

SDL::~SDL() noexcept
{
  SDL_Vulkan_UnloadLibrary();
  SDL_Quit();
}

Window::Window()
    : mWindow {SDL_CreateWindow(
          "Grace",
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          800,
          600,
          SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN)}
{
  if (!mWindow) {
    throw std::runtime_error {"Could not create window"};
  }
}

Window::~Window() noexcept
{
  SDL_DestroyWindow(mWindow);
}

}  // namespace grace::demo
