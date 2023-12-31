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

#include "grace/extras/window.hpp"

#ifdef GRACE_USE_SDL2

#include <memory>  // unique_ptr

#ifdef _MSC_VER
#include <SDL_syswm.h>
#include <dwmapi.h>
#endif  // _MSC_VER

#include "grace/common.hpp"

namespace grace {

void WindowDeleter::operator()(SDL_Window* window) noexcept
{
  SDL_DestroyWindow(window);
}

Window::Window(SDL_Window* window) noexcept
    : mWindow {window}
{
}

void Window::show()
{
  SDL_ShowWindow(mWindow.get());
}

void Window::hide()
{
  SDL_HideWindow(mWindow.get());
}

auto Window::get_size_in_pixels() const -> VkExtent2D
{
  int width = 0;
  int height = 0;
  SDL_GetWindowSizeInPixels(mWindow.get(), &width, &height);
  return {static_cast<uint32>(width), static_cast<uint32>(height)};
}

auto Window::get_size() const -> VkExtent2D
{
  int width = 0;
  int height = 0;
  SDL_GetWindowSize(mWindow.get(), &width, &height);
  return {static_cast<uint32>(width), static_cast<uint32>(height)};
}

auto Window::make(const char* title,
                  const int width,
                  const int height,
                  const uint32 flags) -> Window
{
  return Window {SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  width,
                                  height,
                                  flags)};
}

void enable_dark_title_bar([[maybe_unused]] SDL_Window* window)
{
#ifdef _MSC_VER
  SDL_SysWMinfo wm_info = {};
  SDL_VERSION(&wm_info.version)

  if (SDL_GetWindowWMInfo(window, &wm_info) && wm_info.subsystem == SDL_SYSWM_WINDOWS) {
    auto object_deleter = [](void* obj) { SDL_UnloadObject(obj); };
    using UniqueObject = std::unique_ptr<void, decltype(object_deleter)>;

    UniqueObject dwmapi {SDL_LoadObject("dwmapi.dll")};
    if (!dwmapi) {
      return;
    }

    using DwmSetWindowAttributeFn = HRESULT (*)(HWND, DWORD, LPCVOID, DWORD);
    auto DwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
        SDL_LoadFunction(dwmapi.get(), "DwmSetWindowAttribute"));

    if (DwmSetWindowAttribute) {
      HWND hwnd = wm_info.info.win.window;
      BOOL mode = 1;
      DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &mode, sizeof mode);
    }
  }
#endif  // _MSC_VER
}

}  // namespace grace

#endif  // GRACE_USE_SDL2
