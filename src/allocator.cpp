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
