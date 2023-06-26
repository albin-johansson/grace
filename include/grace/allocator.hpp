#pragma once

#include <memory>  // unique_ptr

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "grace/version.hpp"

namespace grace {

struct AllocatorDeleter final {
  void operator()(VmaAllocator allocator) noexcept;
};

using UniqueAllocator = std::unique_ptr<VmaAllocator_T, AllocatorDeleter>;

struct AllocatorResult final {
  UniqueAllocator ptr;
  VkResult status;
};

/**
 * Attempts to create a Vulkan memory allocator from the VMA library.
 *
 * \param instance       the associated instance.
 * \param gpu            the associated physical device.
 * \param device         the associated logical device.
 * \param vulkan_version the target Vulkan version.
 *
 * \return a Vulkan allocator.
 */
[[nodiscard]] auto make_allocator(VkInstance instance,
                                  VkPhysicalDevice gpu,
                                  VkDevice device,
                                  const ApiVersion& vulkan_version = {1, 2})
    -> AllocatorResult;

}  // namespace grace
