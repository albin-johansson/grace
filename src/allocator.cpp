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

#include "grace/allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace grace {

auto make_allocation_info(const VkMemoryPropertyFlags required_mem_props,
                          const VkMemoryPropertyFlags preferred_mem_props,
                          const VmaAllocationCreateFlags alloc_flags,
                          const VmaMemoryUsage memory_usage) -> VmaAllocationCreateInfo
{
  return {
      .flags = alloc_flags,
      .usage = memory_usage,
      .requiredFlags = required_mem_props,
      .preferredFlags = preferred_mem_props,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0.0f,
  };
}

void AllocatorDeleter::operator()(VmaAllocator allocator) noexcept
{
  vmaDestroyAllocator(allocator);
}

auto make_allocator(VkInstance instance,
                    VkPhysicalDevice gpu,
                    VkDevice device,
                    const ApiVersion& vulkan_version,
                    VkResult* result) -> UniqueAllocator
{
  VmaAllocatorCreateInfo allocator_info = {};
  allocator_info.instance = instance;
  allocator_info.physicalDevice = gpu;
  allocator_info.device = device;
  allocator_info.vulkanApiVersion = to_u32(vulkan_version);

  VmaAllocator allocator = VK_NULL_HANDLE;
  const auto status = vmaCreateAllocator(&allocator_info, &allocator);

  if (result) {
    *result = status;
  }

  return UniqueAllocator {allocator};
}

}  // namespace grace
