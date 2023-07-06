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

#include <concepts>  // default_initializable, destructible, movable, copyable, ...
#include <optional>  // optional

#include <SDL2/SDL.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "grace/allocator.hpp"
#include "grace/device.hpp"
#include "grace/extras/window.hpp"
#include "grace/instance.hpp"
#include "grace/surface.hpp"

namespace grace {

struct TestContext final {
  Window window;
  Instance instance;
  Surface surface;
  VkPhysicalDevice gpu {VK_NULL_HANDLE};
  Device device;
  Allocator allocator;
};

[[nodiscard]] auto make_test_context() -> TestContext;

// clang-format off

template <typename T, typename VulkanHandle>
concept WrapperType = std::default_initializable<T> &&         //
                      std::destructible<T> &&                  //
                      std::movable<T> &&                       //
                      !std::copyable<T> &&                     //
                      std::convertible_to<T, VulkanHandle> &&  //
                      std::convertible_to<T, bool> &&          //
                      requires(T obj) {
                        obj.destroy();
                        { obj.get() } -> std::same_as<VulkanHandle>;
                      };

// clang-format on

};  // namespace grace

// Shorthand for creating a test fixture that properly configures a test context.
#define GRACE_TEST_FIXTURE(Name)                            \
  class Name : public testing::Test {                       \
   public:                                                  \
    static void SetUpTestSuite()                            \
    {                                                       \
      mCtx = grace::make_test_context();                    \
      mWindow = mCtx->window.get();                         \
      mInstance = mCtx->instance.get();                     \
      mSurface = mCtx->surface.get();                       \
      mGPU = mCtx->gpu;                                     \
      mDevice = mCtx->device.get();                         \
      mAllocator = mCtx->allocator.get();                   \
    }                                                       \
                                                            \
    static void TearDownTestSuite()                         \
    {                                                       \
      mCtx.reset();                                         \
      mWindow = nullptr;                                    \
      mInstance = VK_NULL_HANDLE;                           \
      mSurface = VK_NULL_HANDLE;                            \
      mGPU = VK_NULL_HANDLE;                                \
      mDevice = VK_NULL_HANDLE;                             \
      mAllocator = VK_NULL_HANDLE;                          \
    }                                                       \
                                                            \
   private:                                                 \
    inline static std::optional<grace::TestContext> mCtx;   \
                                                            \
   protected:                                               \
    inline static SDL_Window* mWindow {};                   \
    inline static VkInstance mInstance {VK_NULL_HANDLE};    \
    inline static VkSurfaceKHR mSurface {VK_NULL_HANDLE};   \
    inline static VkPhysicalDevice mGPU {VK_NULL_HANDLE};   \
    inline static VkDevice mDevice {VK_NULL_HANDLE};        \
    inline static VmaAllocator mAllocator {VK_NULL_HANDLE}; \
  }
