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

using UniqueAllocator = std::unique_ptr<VmaAllocator_T, AllocatorDeleter>;

/**
 * Attempts to create a Vulkan memory allocator from the VMA library.
 *
 * \param      instance       the associated instance.
 * \param      gpu            the associated physical device.
 * \param      device         the associated logical device.
 * \param      vulkan_version the target Vulkan version.
 * \param[out] result         the resulting error code.
 *
 * \return a Vulkan allocator on success; a null pointer on failure.
 */
[[nodiscard]] auto make_allocator(VkInstance instance,
                                  VkPhysicalDevice gpu,
                                  VkDevice device,
                                  const ApiVersion& vulkan_version = {1, 2},
                                  VkResult* result = nullptr) -> UniqueAllocator;

}  // namespace grace
