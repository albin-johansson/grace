#include "grace/allocator.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace grace {

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
