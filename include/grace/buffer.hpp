#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common.hpp"
#include "context.hpp"

namespace grace {

[[nodiscard]] auto make_buffer_info(uint64 size, VkBufferUsageFlags buffer_usage)
    -> VkBufferCreateInfo;

/// A Vulkan buffer that automatically manages its associated memory.
class Buffer final {
 public:
  Buffer() noexcept = default;

  Buffer(VmaAllocator allocator, VkBuffer buffer, VmaAllocation allocation) noexcept;

  ~Buffer() noexcept;

  Buffer(Buffer&& other) noexcept;
  Buffer(const Buffer& other) = delete;

  Buffer& operator=(Buffer&& other) noexcept;
  Buffer& operator=(const Buffer& other) = delete;

  /**
   * Creates an empty buffer.
   *
   * \param      allocator       the associated allocator.
   * \param      buffer_info     the buffer information.
   * \param      allocation_info the allocation information.
   * \param[out] result          the resulting error code.
   *
   * \return a potentially null buffer.
   */
  [[nodiscard]] static auto make(VmaAllocator allocator,
                                 const VkBufferCreateInfo& buffer_info,
                                 const VmaAllocationCreateInfo& allocation_info,
                                 VkResult* result = nullptr) -> Buffer;

  /**
   * Creates an empty buffer.
   *
   * \param      allocator           the associated allocator.
   * \param      size                the size of the buffer in bytes.
   * \param      buffer_usage        buffer usage flags.
   * \param      required_mem_props  properties required of associated memory.
   * \param      preferred_mem_props properties preferably featured by associated memory.
   * \param      allocation_flags    VMA allocation flags.
   * \param      memory_usage        allocation memory usage flags.
   * \param[out] result              the resulting error code.
   *
   * \return a potentially null buffer.
   */
  [[nodiscard]] static auto make(VmaAllocator allocator,
                                 uint64 size,
                                 VkBufferUsageFlags buffer_usage,
                                 VkMemoryPropertyFlags required_mem_props,
                                 VkMemoryPropertyFlags preferred_mem_props,
                                 VmaAllocationCreateFlags allocation_flags = 0,
                                 VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
                                 VkResult* result = nullptr) -> Buffer;

  /**
   * Creates an empty buffer suitable for use as a staging buffer.
   *
   * \details This function will automatically include the
   *          `VK_BUFFER_USAGE_TRANSFER_SRC_BIT` buffer usage flag.
   *
   * \param      allocator    the associated allocator.
   * \param      size         the total size of the buffer in bytes.
   * \param      buffer_usage buffer usage hint.
   * \param[out] result       the resulting error code.
   *
   * \return a potentially null buffer.
   */
  [[nodiscard]] static auto for_staging(VmaAllocator allocator,
                                        uint64 size,
                                        VkBufferUsageFlags buffer_usage = 0,
                                        VkResult* result = nullptr) -> Buffer;

  /**
   * Creates an empty buffer suitable for use as a uniform buffer.
   *
   * \param      allocator the associated allocator.
   * \param      size      the total size of the buffer in bytes.
   * \param[out] result    the resulting error code.
   *
   * \return a potentially null buffer.
   */
  [[nodiscard]] static auto for_uniforms(VmaAllocator allocator,
                                         uint64 size,
                                         VkResult* result = nullptr) -> Buffer;

  /**
   * Creates an empty device (GPU) buffer.
   *
   * \details This function will automatically include the
   *          `VK_BUFFER_USAGE_TRANSFER_DST_BIT` buffer usage flag.
   *
   * \param      allocator    the associated allocator.
   * \param      size         the total size of the buffer in bytes.
   * \param      buffer_usage buffer usage hint.
   * \param[out] result       the resulting error code.
   *
   * \return a potentially null buffer.
   */
  [[nodiscard]] static auto for_device(VmaAllocator allocator,
                                       uint64 size,
                                       VkBufferUsageFlags buffer_usage,
                                       VkResult* result = nullptr) -> Buffer;

  /**
   * Creates a device (GPU) buffer filled with the specified data.
   *
   * \param      ctx          the associated command context.
   * \param      allocator    the associated allocator.
   * \param      data         the data to store in the buffer.
   * \param      data_size    the size of the data in bytes.
   * \param      buffer_usage buffer usage hint.
   * \param[out] result       the resulting error code.
   *
   * \return a potentially null buffer.
   */
  [[nodiscard]] auto for_device_with_data(const CommandContext& ctx,
                                          VmaAllocator allocator,
                                          const void* data,
                                          uint64 data_size,
                                          VkBufferUsageFlags buffer_usage,
                                          VkResult* result = nullptr) -> Buffer;

  /**
   * Updates the contents of the buffer.
   *
   * \note This function is only usable for buffers that are host-visible (accessible by
   *       the CPU), such as uniform buffers.
   *
   * \param data      the data to store in the buffer.
   * \param data_size the size of the data in bytes.
   *
   * \return `VK_SUCCESS` if the buffer was successfully updated, or an error otherwise.
   */
  auto set_data(const void* data, uint64 data_size) -> VkResult;

  [[nodiscard]] auto get() noexcept -> VkBuffer { return mBuffer; }
  [[nodiscard]] auto allocator() noexcept -> VmaAllocator { return mAllocator; }
  [[nodiscard]] auto allocation() noexcept -> VmaAllocation { return mAllocation; }

  /// Indicates whether the buffer features a non-null buffer handle.
  [[nodiscard]] auto has_value() const noexcept -> bool
  {
    return mBuffer != VK_NULL_HANDLE;
  }

  [[nodiscard]] explicit operator bool() const noexcept { return has_value(); }

 private:
  VmaAllocator mAllocator {VK_NULL_HANDLE};
  VkBuffer mBuffer {VK_NULL_HANDLE};
  VmaAllocation mAllocation {VK_NULL_HANDLE};

  void _destroy() noexcept;
};

}  // namespace grace
