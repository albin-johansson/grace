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

#include "grace/descriptor_pool.hpp"

namespace grace {

auto make_descriptor_pool_info(const uint32 max_set_count,
                               const VkDescriptorPoolSize* pool_sizes,
                               const uint32 pool_size_count,
                               const VkDescriptorPoolCreateFlags flags)
    -> VkDescriptorPoolCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
      .maxSets = max_set_count,
      .poolSizeCount = pool_size_count,
      .pPoolSizes = pool_sizes,
  };
}

auto make_descriptor_set_alloc_info(VkDescriptorPool pool,
                                    const uint32 descriptor_set_count,
                                    const VkDescriptorSetLayout* layouts)
    -> VkDescriptorSetAllocateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = nullptr,
      .descriptorPool = pool,
      .descriptorSetCount = descriptor_set_count,
      .pSetLayouts = layouts,
  };
}

DescriptorPool::DescriptorPool(VkDevice device, VkDescriptorPool descriptor_pool) noexcept
    : mDevice {device},
      mDescriptorPool {descriptor_pool}
{
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : mDevice {other.mDevice},
      mDescriptorPool {other.mDescriptorPool}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mDescriptorPool = VK_NULL_HANDLE;
}

auto DescriptorPool::operator=(DescriptorPool&& other) noexcept -> DescriptorPool&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mDescriptorPool = other.mDescriptorPool;

    other.mDevice = VK_NULL_HANDLE;
    other.mDescriptorPool = VK_NULL_HANDLE;
  }

  return *this;
}

DescriptorPool::~DescriptorPool() noexcept
{
  destroy();
}

void DescriptorPool::destroy() noexcept
{
  if (mDescriptorPool != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
    mDescriptorPool = VK_NULL_HANDLE;
  }
}

auto DescriptorPool::make(VkDevice device,
                          const VkDescriptorPoolCreateInfo& pool_info,
                          VkResult* result) -> DescriptorPool
{
  VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
  const auto status =
      vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return DescriptorPool {device, descriptor_pool};
  }

  return {};
}

auto DescriptorPool::make(VkDevice device,
                          const uint32 max_set_count,
                          const VkDescriptorPoolSize* pool_sizes,
                          const uint32 pool_size_count,
                          const VkDescriptorPoolCreateFlags flags,
                          VkResult* result) -> DescriptorPool
{
  const auto pool_info =
      make_descriptor_pool_info(max_set_count, pool_sizes, pool_size_count, flags);
  return DescriptorPool::make(device, pool_info, result);
}

}  // namespace grace
