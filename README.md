# grace

A small library that makes working with Vulkan a little easier.

Note, this is not a Vulkan wrapper library. Instead, this library provides various utilities that aim to ease the burden
of getting Vulkan applications up and running. Below is a minimalistic example of how the library can be used that
excludes aspects such as proper error checking.

```C++
#include <grace/grace.hpp>

int main(int argc, char* argv[])
{
  const grace::SDL sdl;
  auto window = grace::make_window("Grace", 800, 600);

  const std::vector enabled_layers = {"VK_LAYER_KHRONOS_validation"};

  // Create instance
  const auto instance_extensions = grace::get_required_instance_extensions(window.get());
  auto instance = grace::make_instance("Grace", enabled_layers, instance_extensions);

  // Create surface
  auto surface = grace::make_surface(window.get(), instance.get());
  
  // Pick physical device
  auto gpu_filter = [](VkPhysicalDevice gpu, VkSurfaceKHR surface) { return true; };
  auto gpu_rater = [](VkPhysicalDevice gpu) { return 1; };
  auto gpu = grace::pick_physical_device(instance, surface, gpu_filter, gpu_rater);

  // Create logical device
  grace::DeviceSpec device_spec;
  device_spec.layers = enabled_layers;
  auto device = grace::make_device(gpu, surface.get(), device_spec);

  // Create allocator
  auto allocator = grace::make_allocator(instance.get(), gpu, device.get());

  // TODO swapchain
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
