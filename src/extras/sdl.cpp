/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "grace/extras/sdl.hpp"

#ifdef GRACE_USE_SDL2

#include <iostream>   // cerr
#include <stdexcept>  // runtime_error

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

namespace grace {

SDL::SDL()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
    throw std::runtime_error {"Could not initialize SDL"};
  }

  if (SDL_Vulkan_LoadLibrary(nullptr) == -1) {
    std::cerr << "Could not load Vulkan library: " << SDL_GetError() << '\n';

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
