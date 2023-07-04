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

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_semaphore_info(VkSemaphoreCreateFlags flags = 0)
    -> VkSemaphoreCreateInfo;

class Semaphore final {
 public:
  Semaphore() noexcept = default;

  Semaphore(VkDevice device, VkSemaphore semaphore) noexcept;

  Semaphore(Semaphore&& other) noexcept;
  Semaphore(const Semaphore& other) = delete;

  auto operator=(Semaphore&& other) noexcept -> Semaphore&;
  auto operator=(const Semaphore& other) -> Semaphore& = delete;

  ~Semaphore() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkSemaphoreCreateInfo& semaphore_info,
                                 VkResult* result = nullptr) -> Semaphore;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkSemaphoreCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> Semaphore;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkSemaphore { return mSemaphore; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkSemaphore() noexcept { return mSemaphore; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mSemaphore != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkSemaphore mSemaphore {VK_NULL_HANDLE};
};

}  // namespace grace
