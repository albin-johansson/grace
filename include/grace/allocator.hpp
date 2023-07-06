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

#include <memory>  // unique_ptr

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "grace/version.hpp"

namespace grace {

[[nodiscard]] auto make_allocation_info(
    VkMemoryPropertyFlags required_mem_props,
    VkMemoryPropertyFlags preferred_mem_props,
    VmaAllocationCreateFlags alloc_flags,
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO) -> VmaAllocationCreateInfo;

struct AllocatorDeleter final {
  void operator()(VmaAllocator allocator) noexcept;
};

class Allocator final {
 public:
  /**
   * Attempts to create a Vulkan memory allocator.
   *
   * \param      instance       the associated instance.
   * \param      gpu            the associated physical device.
   * \param      device         the associated logical device.
   * \param      vulkan_version the target Vulkan version.
   * \param[out] result         the resulting error code.
   *
   * \return a potentially null allocator.
   */
  [[nodiscard]] static auto make(VkInstance instance,
                                 VkPhysicalDevice gpu,
                                 VkDevice device,
                                 const ApiVersion& vulkan_version = {1, 2},
                                 VkResult* result = nullptr) -> Allocator;

  Allocator() noexcept = default;

  explicit Allocator(VmaAllocator allocator) noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VmaAllocator { return mAllocator.get(); }

  [[nodiscard]] operator VmaAllocator() noexcept { return mAllocator.get(); }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mAllocator != VK_NULL_HANDLE;
  }

 private:
  std::unique_ptr<VmaAllocator_T, AllocatorDeleter> mAllocator;
};

}  // namespace grace
