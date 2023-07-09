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

[[nodiscard]] auto make_descriptor_pool_info(uint32 max_set_count,
                                             const VkDescriptorPoolSize* pool_sizes,
                                             uint32 pool_size_count,
                                             VkDescriptorPoolCreateFlags flags = 0)
    -> VkDescriptorPoolCreateInfo;

[[nodiscard]] auto make_descriptor_set_alloc_info(VkDescriptorPool pool,
                                                  uint32 descriptor_set_count,
                                                  const VkDescriptorSetLayout* layouts)
    -> VkDescriptorSetAllocateInfo;

class DescriptorPool final {
 public:
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkDescriptorPoolCreateInfo& pool_info,
                                 VkResult* result = nullptr) -> DescriptorPool;

  [[nodiscard]] static auto make(VkDevice device,
                                 uint32 max_set_count,
                                 const VkDescriptorPoolSize* pool_sizes,
                                 uint32 pool_size_count,
                                 VkDescriptorPoolCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> DescriptorPool;

  DescriptorPool() noexcept = default;

  DescriptorPool(VkDevice device, VkDescriptorPool descriptor_pool) noexcept;

  DescriptorPool(DescriptorPool&& other) noexcept;
  DescriptorPool(const DescriptorPool& other) = delete;

  auto operator=(DescriptorPool&& other) noexcept -> DescriptorPool&;
  auto operator=(const DescriptorPool& other) -> DescriptorPool& = delete;

  ~DescriptorPool() noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkDescriptorPool { return mDescriptorPool; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkDescriptorPool() noexcept { return mDescriptorPool; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mDescriptorPool != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkDescriptorPool mDescriptorPool {VK_NULL_HANDLE};
};

}  // namespace grace
