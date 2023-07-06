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

[[nodiscard]] auto make_fence_info(VkFenceCreateFlags flags = 0) -> VkFenceCreateInfo;

class Fence final {
 public:
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkFenceCreateInfo& fence_info,
                                 VkResult* result = nullptr) -> Fence;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkFenceCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> Fence;

  Fence() noexcept = default;

  Fence(Fence&& other) noexcept;
  Fence(const Fence& other) = delete;

  auto operator=(Fence&& other) noexcept -> Fence&;
  auto operator=(const Fence& other) -> Fence& = delete;

  ~Fence() noexcept;

  void destroy() noexcept;

  auto wait(uint64 timeout = kMaxU64) -> VkResult;

  auto reset() -> VkResult;

  [[nodiscard]] auto get() noexcept -> VkFence { return mFence; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkFence() noexcept { return mFence; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mFence != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkFence mFence {VK_NULL_HANDLE};
};

}  // namespace grace
