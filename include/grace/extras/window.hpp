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

#pragma once

#ifdef GRACE_USE_SDL2

#include <memory>  // unique_ptr

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "../common.hpp"

namespace grace {

struct WindowDeleter final {
  void operator()(SDL_Window* window) noexcept;
};

class Window final {
 public:
  [[nodiscard]] static auto make(const char* title,
                                 int width,
                                 int height,
                                 uint32 flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE |
                                                SDL_WINDOW_ALLOW_HIGHDPI |
                                                SDL_WINDOW_VULKAN) -> Window;

  Window() noexcept = default;

  explicit Window(SDL_Window* window) noexcept;

  void show();

  void hide();

  [[nodiscard]] auto get_size_in_pixels() const -> VkExtent2D;

  [[nodiscard]] auto get_size() const -> VkExtent2D;

  [[nodiscard]] auto get() noexcept -> SDL_Window* { return mWindow.get(); }

  [[nodiscard]] operator SDL_Window*() noexcept { return mWindow.get(); }

  [[nodiscard]] explicit operator bool() const noexcept { return mWindow != nullptr; }

 private:
  std::unique_ptr<SDL_Window, WindowDeleter> mWindow;
};

void enable_dark_title_bar(SDL_Window* window);

}  // namespace grace

#endif  // GRACE_USE_SDL2