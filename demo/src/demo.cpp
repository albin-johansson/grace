#include <cstdlib>   // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>  // cout, cerr
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "demo_setup.hpp"

int main()
{
  const grace::demo::SDL sdl;
  grace::demo::Window window;

  const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
  const auto instance_extensions = grace::get_required_instance_extensions(window.data());

  window.show();

  bool running = true;
  while (running) {
    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
  }

  window.hide();

  std::cout << "Exiting...\n";
  return EXIT_SUCCESS;
}
