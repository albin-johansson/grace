# grace

[![CI](https://github.com/albin-johansson/grace/actions/workflows/ci.yml/badge.svg)](https://github.com/albin-johansson/grace/actions/workflows/ci.yml)

A small library that makes working with Vulkan a little easier.

Note, this is not a Vulkan wrapper library. Instead, this library provides various utilities that aim to ease the burden
of getting Vulkan applications up and running. Below is a minimalistic example of how the library can be used that
excludes aspects such as proper error checking.

```C++
#include <grace/grace.hpp>

int main(int argc, char* argv[])
{
  const grace::SDL sdl;
  auto window = grace::Window::make("Grace", 800, 600);

  const std::vector enabled_layers = {"VK_LAYER_KHRONOS_validation"};

  // Create instance
  const auto instance_extensions = grace::get_required_instance_extensions(window);
  auto instance = grace::Instance::make("Grace", enabled_layers, instance_extensions);

  // Create surface
  auto surface = grace::Surface::make(window, instance);
  
  // Pick physical device
  auto gpu_filter = [](VkPhysicalDevice gpu, VkSurfaceKHR surface) { return true; };
  auto gpu_rater = [](VkPhysicalDevice gpu) { return 1; };
  auto gpu = grace::pick_physical_device(instance, surface, gpu_filter, gpu_rater);

  // Create logical device
  const std::vector device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  auto device = grace::Device::make(gpu, surface, enabled_layers, device_extensions);

  // Create allocator
  auto allocator = grace::make_allocator(instance, gpu, device);

  // Create swapchain
  const VkExtent2D image_extent = {800, 600};
  auto surface_format_filter = [](const VkSurfaceFormatKHR& format) { return true; };
  auto present_mode_filter = [](VkPresentModeKHR format) { return true; };
  auto swapchain = grace::Swapchain::make(surface,
                                          gpu,
                                          device,
                                          allocator,
                                          image_extent,
                                          surface_format_filter,
                                          present_mode_filter);

  // TODO render pass
  // TODO sampler
  // TODO pipeline cache
  // TODO pipeline
  // TODO command pool
  // TODO command buffer
}
```

## Installation

The easiest approach is to incorporate this repository as a Git submodule and include the project as a CMake
subdirectory. The library has been designed with this approach in mind. Dependencies are managed via
[Vcpkg](https://github.com/microsoft/vcpkg), refer to the Vcpkg documentation for more details.

## Requirements

* A C++20 compiler
* Vulkan SDK (make sure the `VULKAN_SDK` environment variable is defined!)
* VMA (Vulkan Memory Allocator)
* SDL2 (optional, enabled by setting the `GRACE_USE_SDL2` CMake option to `ON`)
