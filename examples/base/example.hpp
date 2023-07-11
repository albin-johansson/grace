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

#include <cstdlib>    // EXIT_SUCCESS
#include <exception>  // exception
#include <iostream>   // cerr, cout
#include <vector>     // vector

#include <SDL2/SDL.h>
#include <grace/grace.hpp>
#include <vulkan/vulkan.h>

namespace grace::examples {

inline constexpr ApiVersion kTargetVulkanVersion = {1, 2};
inline constexpr usize kMaxFramesInFlight = 2;

#ifdef NDEBUG
inline const std::vector<const char*> kEnabledLayers;
#else
inline const std::vector kEnabledLayers = {"VK_LAYER_KHRONOS_validation"};
#endif  // NDEBUG

struct Frame final {
  VkCommandBuffer cmd_buffer {VK_NULL_HANDLE};
  Fence in_flight_fence;
  Semaphore image_available_semaphore;
  Semaphore render_finished_semaphore;
};

class Example {
  struct ExtensionFunctions final {
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR {nullptr};
  };

 public:
  explicit Example(const char* name);

  virtual ~Example() noexcept = default;

  void start();

 protected:
  virtual void record_commands() {};

  virtual void on_mouse_dragged([[maybe_unused]] float dx, [[maybe_unused]] float dy) {}

  virtual void on_key_pressed([[maybe_unused]] SDL_Scancode key) {}

  virtual void on_event([[maybe_unused]] const SDL_Event& event) {}

  Window mWindow;
  Instance mInstance;
  DebugMessenger mDebugMessenger;
  Surface mSurface;
  VkPhysicalDevice mGPU {VK_NULL_HANDLE};
  Device mDevice;
  VkQueue mGraphicsQueue {VK_NULL_HANDLE};
  VkQueue mPresentQueue {VK_NULL_HANDLE};
  Allocator mAllocator;
  Swapchain mSwapchain;
  RenderPass mRenderPass;
  PipelineCache mPipelineCache;
  CommandPool mGraphicsCommandPool;
  Sampler mSampler;
  std::vector<Frame> mFrames;
  uint32 mFrameIndex {0};

  ExtensionFunctions mFunctions;

 private:
  [[nodiscard]] auto _try_acquire_image() -> bool;
  void _render();
  void _submit_commands();
  void _present_image();
  auto _recreate_swapchain() -> VkResult;
};

}  // namespace grace::examples

#define GRACE_EXAMPLE_DEFINE_MAIN(Class)                       \
  auto main(int, char**)->int                                  \
  {                                                            \
    try {                                                      \
      Class example;                                           \
      example.start();                                         \
    }                                                          \
    catch (const std::exception& e) {                          \
      std::cerr << "Uncaught exception: " << e.what() << '\n'; \
      return EXIT_FAILURE;                                     \
    }                                                          \
                                                               \
    return EXIT_SUCCESS;                                       \
  }
