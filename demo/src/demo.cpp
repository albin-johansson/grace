#include <cstdlib>   // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>  // cout, cerr
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "grace/extras/sdl.hpp"
#include "grace/extras/window.hpp"
#include "grace/instance.hpp"
#include "grace/surface.hpp"

int main()
{
  const grace::SDL sdl;
  auto window = grace::make_window("Grace demo", 800, 600);

  const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
  const auto instance_extensions = grace::get_required_instance_extensions(window.get());

  auto [instance, instance_status] =
      grace::make_instance("Grace demo", layers, instance_extensions);

  if (instance_status != VK_SUCCESS) {
    std::cerr << "Could not create instance: " << instance_status << '\n';
    return EXIT_FAILURE;
  }
  else {
    std::cout << "Successfully created instance\n";
  }

  auto [surface, surface_status] = grace::make_surface(window.get(), instance.get());

  if (surface_status != VK_SUCCESS) {
    std::cerr << "Could not create surface: " << surface_status << '\n';
    return EXIT_FAILURE;
  }
  else {
    std::cout << "Successfully created surface\n";
  }

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
