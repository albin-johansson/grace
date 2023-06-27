#pragma once

#include <optional>  // optional

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common.hpp"
#include "context.hpp"

namespace grace {

struct AllocatedBuffer final {
  VmaAllocator allocator {VK_NULL_HANDLE};
  VkBuffer data {VK_NULL_HANDLE};
  VmaAllocation allocation {VK_NULL_HANDLE};

  AllocatedBuffer() = default;
  ~AllocatedBuffer() noexcept;

  AllocatedBuffer(AllocatedBuffer&& other) noexcept;
  AllocatedBuffer(const AllocatedBuffer&) = delete;

  AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept;
  AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;
};

struct BufferSpec final {
  uint64 size;                                ///< Size of the buffer in bytes.
  VkBufferUsageFlags buffer_usage;            ///< Buffer usage hint.
  VkMemoryPropertyFlags required_mem_props;   ///< Required memory properties.
  VkMemoryPropertyFlags preferred_mem_props;  ///< Preferable memory properties.
  VmaAllocationCreateFlags allocation_flags;  ///< VMA allocation flags.
  VmaMemoryUsage memory_usage;                ///< Allocation memory usage hint.
};

[[nodiscard]] auto make_buffer_info(uint64 size, VkBufferUsageFlags buffer_usage)
    -> VkBufferCreateInfo;

/**
 * Creates an empty buffer.
 *
 * \param      allocator the associated allocator.
 * \param      spec      the buffer specification.
 * \param[out] result    the resulting error code.
 *
 * \return an allocated buffer on success, nothing on failure.
 */
[[nodiscard]] auto make_buffer(VmaAllocator allocator,
                               const BufferSpec& spec,
                               VkResult* result = nullptr)
    -> std::optional<AllocatedBuffer>;

/**
 * Creates an empty buffer suitable for use as a staging buffer.
 *
 * \param      allocator    the associated allocator.
 * \param      size         the total size of the buffer in bytes.
 * \param      buffer_usage buffer usage hint.
 * \param[out] result       the resulting error code.
 *
 * \return a staging buffer on success, nothing on failure.
 */
[[nodiscard]] auto make_staging_buffer(VmaAllocator allocator,
                                       uint64 size,
                                       VkBufferUsageFlags buffer_usage,
                                       VkResult* result = nullptr)
    -> std::optional<AllocatedBuffer>;

/**
 * Creates an empty device (GPU) buffer.
 *
 * \param      allocator    the associated allocator.
 * \param      size         the total size of the buffer in bytes.
 * \param      buffer_usage buffer usage hint.
 * \param[out] result       the resulting error code.
 *
 * \return a device buffer on success, nothing on failure.
 */
[[nodiscard]] auto make_device_buffer(VmaAllocator allocator,
                                      uint64 size,
                                      VkBufferUsageFlags buffer_usage,
                                      VkResult* result = nullptr)
    -> std::optional<AllocatedBuffer>;

/**
 * Creates a device (GPU) buffer with the specified data.
 *
 * \param      ctx          the associated command context.
 * \param      allocator    the associated allocator.
 * \param      data         the data to store in the buffer.
 * \param      data_size    the size of the data in bytes.
 * \param      buffer_usage buffer usage hint.
 * \param[out] result       the resulting error code.
 *
 * \return a device buffer on success, nothing on failure.
 */
[[nodiscard]] auto make_device_buffer(const CommandContext& ctx,
                                      VmaAllocator allocator,
                                      const void* data,
                                      uint64 data_size,
                                      VkBufferUsageFlags buffer_usage,
                                      VkResult* result = nullptr)
    -> std::optional<AllocatedBuffer>;

/**
 * Creates an empty buffer suitable for use as a uniform buffer.
 *
 * \param      allocator the associated allocator.
 * \param      size      the total size of the buffer in bytes.
 * \param[out] result    the resulting error code.
 *
 * \return a uniform buffer on success, nothing on failure.
 */
[[nodiscard]] auto make_uniform_buffer(VmaAllocator allocator,
                                       uint64 size,
                                       VkResult* result = nullptr)
    -> std::optional<AllocatedBuffer>;

/**
 * Updates the contents of a buffer.
 *
 * \note This function is only usable for buffers that are host-visible (accessible by the
 *       CPU), such as uniform buffers.
 *
 * \param buffer    the target buffer.
 * \param data      the data to store in the buffer.
 * \param data_size the size of the data in bytes.
 *
 * \return `VK_SUCCESS` if the buffer was successfully updated, an error otherwise.
 */
auto set_buffer_data(AllocatedBuffer& buffer, const void* data, uint64 data_size)
    -> VkResult;

}  // namespace grace
