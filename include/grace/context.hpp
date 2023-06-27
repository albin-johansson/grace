#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace grace {

struct CommandContext final {
  VkDevice device {VK_NULL_HANDLE};
  VkQueue queue {VK_NULL_HANDLE};
  VkCommandPool cmd_pool {VK_NULL_HANDLE};
};

}  // namespace grace
