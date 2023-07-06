# grace

[![CI](https://github.com/albin-johansson/grace/actions/workflows/ci.yml/badge.svg)](https://github.com/albin-johansson/grace/actions/workflows/ci.yml)

A small library that makes working with Vulkan a little easier.

Note, this is not a Vulkan wrapper library. Instead, this library provides various utilities that aim to ease the burden
of getting Vulkan applications up and running. Below is a minimalistic example of how the library can be used that
excludes aspects such as proper error checking.

```C++
#include <vector>  // vector

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
  const std::vector device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                         VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};
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

  // Create render pass
  const auto subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  const auto main_subpass_access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  const auto main_subpass_dependency = 
      grace::make_subpass_dependency(VK_SUBPASS_EXTERNAL,
                                     0,
                                     subpass_stages,
                                     subpass_stages,
                                     0,
                                     main_subpass_access);

  auto render_pass = 
      grace::RenderPassBuilder {device}
          .color_attachment(swapchain.info().image_format)
          .depth_attachment(swapchain.get_depth_buffer_format())
          .begin_subpass()
          .set_color_attachment(0)
          .set_depth_attachment(1)
          .end_subpass()
          .subpass_dependency(main_subpass_dependency)
          .build();

  // Create pipeline cache
  auto pipeline_cache = grace::PipelineCache::make(device);

  // Create descriptor set layout
  auto descriptor_set_layout =
      grace::DescriptorSetLayoutBuilder {device}
          .use_push_descriptors()
          .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
          .build();
  
  // Create pipeline layout
  auto pipeline_layout =
      grace::PipelineLayoutBuilder {device}
          .descriptor_set_layout(descriptor_set_layout)
          .build();

  // Create pipeline
  auto pipeline =
      grace::GraphicsPipelineBuilder {device}
          .with_layout(pipeline_layout)
          .with_cache(pipeline_cache)
          .with_render_pass(render_pass, 0)
          .vertex_shader("shaders/demo.vert.spv")
          .fragment_shader("shaders/demo.frag.spv")
          .color_blend_attachment(false)
          .viewport(0, 0, image_extent.width, image_extent.height)
          .scissor(0, 0, image_extent.width, image_extent.height)
          .build();

  // Create sampler
  auto sampler = grace::Sampler::make(device,
                                      gpu,
                                      VK_FILTER_LINEAR,
                                      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

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
