#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace grace {

void SetGlobalAllocator(VmaAllocator allocator) noexcept;
void SetGlobalInstance(VkInstance instance) noexcept;
void SetGlobalDevice(VkDevice device) noexcept;
void SetGlobalGraphicsCommandPool(VkCommandPool cmd_pool) noexcept;
void SetGlobalPresentationCommandPool(VkCommandPool cmd_pool) noexcept;

[[nodiscard]] auto GetGlobalAllocator() noexcept -> VmaAllocator;
[[nodiscard]] auto GetGlobalInstance() noexcept -> VkInstance;
[[nodiscard]] auto GetGlobalDevice() noexcept -> VkDevice;
[[nodiscard]] auto GetGlobalGraphicsCommandPool() noexcept -> VkCommandPool;
[[nodiscard]] auto GetGlobalPresentationCommandPool() noexcept -> VkCommandPool;

}  // namespace grace
