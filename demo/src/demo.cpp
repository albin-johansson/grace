#include <cstdlib>   // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>  // cout, cerr
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "grace/extras/sdl.hpp"
#include "grace/extras/window.hpp"

int main()
{
  const grace::SDL sdl;
  auto window = grace::make_window("Grace demo", 800, 600);

  const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
  const auto instance_extensions = grace::get_required_instance_extensions(window.get());

  SDL_ShowWindow(window.get());

  bool running = true;
  while (running) {
    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
  }

  SDL_HideWindow(window.get());

  std::cout << "Exiting...\n";
  return EXIT_SUCCESS;
}
