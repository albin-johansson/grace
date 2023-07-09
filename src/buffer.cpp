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

#include "grace/buffer.hpp"

#include <algorithm>  // min
#include <cstring>    // memcpy

#include "grace/allocator.hpp"
#include "grace/command_pool.hpp"

namespace grace {

auto make_buffer_info(const uint64 size, const VkBufferUsageFlags buffer_usage)
    -> VkBufferCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = size,
      .usage = buffer_usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };
}

Buffer::Buffer(VmaAllocator allocator, VkBuffer buffer, VmaAllocation allocation) noexcept
    : mAllocator {allocator},
      mBuffer {buffer},
      mAllocation {allocation}
{
}

Buffer::Buffer(Buffer&& other) noexcept
    : mAllocator {other.mAllocator},
      mBuffer {other.mBuffer},
      mAllocation {other.mAllocation}
{
  other.mAllocator = VK_NULL_HANDLE;
  other.mBuffer = VK_NULL_HANDLE;
  other.mAllocation = VK_NULL_HANDLE;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
  if (this != &other) {
    destroy();

    mAllocator = other.mAllocator;
    mBuffer = other.mBuffer;
    mAllocation = other.mAllocation;

    other.mAllocator = VK_NULL_HANDLE;
    other.mBuffer = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
  }

  return *this;
}

Buffer::~Buffer() noexcept
{
  destroy();
}

void Buffer::destroy() noexcept
{
  if (mBuffer != VK_NULL_HANDLE) {
    vmaDestroyBuffer(mAllocator, mBuffer, mAllocation);
    mBuffer = VK_NULL_HANDLE;
    mAllocation = VK_NULL_HANDLE;
  }
}

auto Buffer::make(VmaAllocator allocator,
                  const VkBufferCreateInfo& buffer_info,
                  const VmaAllocationCreateInfo& allocation_info,
                  VkResult* result) -> Buffer
{
  VkBuffer buffer = VK_NULL_HANDLE;
  VmaAllocation allocation = VK_NULL_HANDLE;
  const auto status = vmaCreateBuffer(allocator,
                                      &buffer_info,
                                      &allocation_info,
                                      &buffer,
                                      &allocation,
                                      nullptr);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return Buffer {allocator, buffer, allocation};
  }

  return Buffer {};
}

auto Buffer::make(VmaAllocator allocator,
                  const uint64 size,
                  const VkBufferUsageFlags buffer_usage,
                  const VkMemoryPropertyFlags required_mem_props,
                  const VkMemoryPropertyFlags preferred_mem_props,
                  const VmaAllocationCreateFlags allocation_flags,
                  const VmaMemoryUsage memory_usage,
                  VkResult* result) -> Buffer
{
  const auto buffer_info = make_buffer_info(size, buffer_usage);
  const auto allocation_info = make_allocation_info(required_mem_props,
                                                    preferred_mem_props,
                                                    allocation_flags,
                                                    memory_usage);
  return make(allocator, buffer_info, allocation_info, result);
}

auto Buffer::for_staging(VmaAllocator allocator,
                         const uint64 size,
                         VkBufferUsageFlags buffer_usage,
                         VkResult* result) -> Buffer
{
  buffer_usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  const VkMemoryPropertyFlags required_mem_props =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  const VkMemoryPropertyFlags preferred_mem_props = 0;
  const VmaAllocationCreateFlags allocation_flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  return make(allocator,
              size,
              buffer_usage,
              required_mem_props,
              preferred_mem_props,
              allocation_flags,
              VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
              result);
}

auto Buffer::for_uniforms(VmaAllocator allocator, const uint64 size, VkResult* result)
    -> Buffer
{
  const VkMemoryPropertyFlags required_mem_props =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  const VkMemoryPropertyFlags preferred_mem_props = 0;
  const VmaAllocationCreateFlags allocation_flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

  return make(allocator,
              size,
              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
              required_mem_props,
              preferred_mem_props,
              allocation_flags,
              VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
              result);
}

auto Buffer::for_device(VmaAllocator allocator,
                        const uint64 size,
                        VkBufferUsageFlags buffer_usage,
                        VkResult* result) -> Buffer
{
  buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  const VkMemoryPropertyFlags required_mem_props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  const VkMemoryPropertyFlags preferred_mem_props = 0;
  const VmaAllocationCreateFlags allocation_flags = 0;

  return make(allocator,
              size,
              buffer_usage,
              required_mem_props,
              preferred_mem_props,
              allocation_flags,
              VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
              result);
}

auto Buffer::for_device(const CommandContext& ctx,
                        VmaAllocator allocator,
                        const void* data,
                        const uint64 data_size,
                        const VkBufferUsageFlags buffer_usage,
                        VkResult* result) -> Buffer
{
  auto staging_buffer = Buffer::for_staging(allocator, data_size, buffer_usage, result);
  if (!staging_buffer) {
    return Buffer {};
  }

  if (const auto status = staging_buffer.set_data(data, data_size);
      status != VK_SUCCESS) {
    if (result) {
      *result = status;
    }

    return Buffer {};
  }

  auto device_buffer = Buffer::for_device(allocator, data_size, buffer_usage, result);
  if (!device_buffer) {
    return Buffer {};
  }

  execute_now(ctx, [&](VkCommandBuffer cmd_buffer) {
    const VkBufferCopy region = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = data_size,
    };
    vkCmdCopyBuffer(cmd_buffer,
                    staging_buffer.mBuffer,
                    device_buffer.mBuffer,
                    1,
                    &region);
  });

  return device_buffer;
}

auto Buffer::set_data(const void* data, const uint64 data_size) -> VkResult
{
  void* mapped_data = nullptr;

  const auto map_result = vmaMapMemory(mAllocator, mAllocation, &mapped_data);
  if (map_result != VK_SUCCESS) {
    return map_result;
  }

  VmaAllocationInfo allocation_info = {};
  vmaGetAllocationInfo(mAllocator, mAllocation, &allocation_info);

  // Transfer the data, making sure not to write too much data into the buffer
  const auto allocation_size = static_cast<uint64>(allocation_info.size);
  std::memcpy(mapped_data, data, std::min(data_size, allocation_size));

  vmaUnmapMemory(mAllocator, mAllocation);

  return VK_SUCCESS;
}

void Buffer::bind_as_vertex_buffer(VkCommandBuffer cmd_buffer)
{
  const VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &mBuffer, offsets);
}

void Buffer::bind_as_index_buffer(VkCommandBuffer cmd_buffer,
                                  const VkIndexType index_type)
{
  vkCmdBindIndexBuffer(cmd_buffer, mBuffer, 0, index_type);
}

}  // namespace grace
