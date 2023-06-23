#include "grace/extras/sdl.hpp"

#ifdef GRACE_USE_SDL2

#include <stdexcept>  // runtime_error

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace grace {

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

}  // namespace grace

#endif  // GRACE_USE_SDL2
