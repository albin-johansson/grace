#include "grace/buffer.hpp"

#include <algorithm>  // min
#include <cstring>    // memcpy

#include "grace/command_pool.hpp"

namespace grace {
namespace {

void _destroy_buffer(VmaAllocator allocator,
                     VkBuffer buffer,
                     VmaAllocation allocation) noexcept
{
  if (buffer != VK_NULL_HANDLE) {
    vmaDestroyBuffer(allocator, buffer, allocation);
  }
}

}  // namespace

AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other) noexcept
    : allocator {other.allocator},
      data {other.data},
      allocation {other.allocation}
{
  other.allocator = VK_NULL_HANDLE;
  other.data = VK_NULL_HANDLE;
  other.allocation = VK_NULL_HANDLE;
}

AllocatedBuffer& AllocatedBuffer::operator=(AllocatedBuffer&& other) noexcept
{
  if (this != &other) {
    _destroy_buffer(allocator, data, allocation);

    allocator = other.allocator;
    data = other.data;
    allocation = other.allocation;

    other.allocator = VK_NULL_HANDLE;
    other.data = VK_NULL_HANDLE;
    other.allocation = VK_NULL_HANDLE;
  }

  return *this;
}

AllocatedBuffer::~AllocatedBuffer() noexcept
{
  _destroy_buffer(allocator, data, allocation);
}

auto make_buffer_info(const uint64 size, const VkBufferUsageFlags buffer_usage)
    -> VkBufferCreateInfo
{
  VkBufferCreateInfo buffer_info = {};

  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = buffer_usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  return buffer_info;
}

auto make_buffer(VmaAllocator allocator, const BufferSpec& spec, VkResult* result)
    -> std::optional<AllocatedBuffer>
{
  const auto buffer_info = make_buffer_info(spec.size, spec.buffer_usage);

  VmaAllocationCreateInfo alloc_info = {};
  alloc_info.flags = spec.allocation_flags;
  alloc_info.requiredFlags = spec.required_mem_props;
  alloc_info.preferredFlags = spec.preferred_mem_props;
  alloc_info.usage = spec.memory_usage;

  AllocatedBuffer buffer;
  buffer.allocator = allocator;

  const auto status = vmaCreateBuffer(allocator,
                                      &buffer_info,
                                      &alloc_info,
                                      &buffer.data,
                                      &buffer.allocation,
                                      nullptr);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return buffer;
  }

  return std::nullopt;
}

auto make_staging_buffer(VmaAllocator allocator,
                         const uint64 size,
                         const VkBufferUsageFlags buffer_usage,
                         VkResult* result) -> std::optional<AllocatedBuffer>
{
  BufferSpec spec = {};
  spec.size = size;
  spec.buffer_usage = buffer_usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  spec.required_mem_props =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  spec.allocation_flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  spec.memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

  return make_buffer(allocator, spec, result);
}

auto make_device_buffer(VmaAllocator allocator,
                        const uint64 size,
                        const VkBufferUsageFlags buffer_usage,
                        VkResult* result) -> std::optional<AllocatedBuffer>
{
  BufferSpec spec = {};
  spec.size = size;
  spec.buffer_usage = buffer_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  spec.required_mem_props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  spec.memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

  return make_buffer(allocator, spec, result);
}

auto make_device_buffer(const CommandContext& ctx,
                        VmaAllocator allocator,
                        const void* data,
                        const uint64 data_size,
                        const VkBufferUsageFlags buffer_usage,
                        VkResult* result) -> std::optional<AllocatedBuffer>
{
  auto staging_buffer = make_staging_buffer(allocator, data_size, buffer_usage, result);
  if (!staging_buffer) {
    return std::nullopt;
  }

  if (const auto status = set_buffer_data(*staging_buffer, data, data_size);
      status != VK_SUCCESS) {
    if (result) {
      *result = status;
    }

    return std::nullopt;
  }

  auto device_buffer = make_device_buffer(allocator, data_size, buffer_usage, result);
  if (!device_buffer) {
    return std::nullopt;
  }

  execute_now(ctx.device, ctx.queue, ctx.cmd_pool, [&](VkCommandBuffer cmd_buffer) {
    VkBufferCopy region = {};
    region.size = data_size;
    vkCmdCopyBuffer(cmd_buffer, staging_buffer->data, device_buffer->data, 1, &region);
  });

  return device_buffer;
}

auto make_uniform_buffer(VmaAllocator allocator, const uint64 size, VkResult* result)
    -> std::optional<AllocatedBuffer>
{
  BufferSpec spec = {};
  spec.size = size;
  spec.buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  spec.required_mem_props =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  spec.allocation_flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
  spec.memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

  return make_buffer(allocator, spec, result);
}

auto set_buffer_data(AllocatedBuffer& buffer, const void* data, const uint64 data_size)
    -> VkResult
{
  void* mapped_data = nullptr;
  if (const auto result = vmaMapMemory(buffer.allocator, buffer.allocation, &mapped_data);
      result != VK_SUCCESS) {
    return result;
  }

  VmaAllocationInfo allocation_info = {};
  vmaGetAllocationInfo(buffer.allocator, buffer.allocation, &allocation_info);

  // Transfer the data, making sure not to write too much data into the buffer
  const auto allocation_size = static_cast<uint64>(allocation_info.size);
  std::memcpy(mapped_data, data, std::min(data_size, allocation_size));

  vmaUnmapMemory(buffer.allocator, buffer.allocation);

  return VK_SUCCESS;
}

}  // namespace grace
