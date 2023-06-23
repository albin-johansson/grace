#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace grace {

struct Context final {
  VkInstance instance {VK_NULL_HANDLE};
  VkPhysicalDevice gpu {VK_NULL_HANDLE};
  VkDevice device {VK_NULL_HANDLE};
  VmaAllocator allocator {VK_NULL_HANDLE};
  VkCommandPool graphics_command_pool {VK_NULL_HANDLE};
};

}  // namespace grace
