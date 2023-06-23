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

  std::cout << "Exiting...\n";
  return EXIT_SUCCESS;
}
