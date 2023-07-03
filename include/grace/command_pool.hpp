#pragma once

#include <functional>  // function

#include <vulkan/vulkan.h>

#include "common.hpp"
#include "context.hpp"

namespace grace {

using CommandBufferCallback = std::function<void(VkCommandBuffer)>;

[[nodiscard]] auto make_command_pool_info(uint32 queue_family_index,
                                          VkCommandPoolCreateFlags flags = 0)
    -> VkCommandPoolCreateInfo;

[[nodiscard]] auto make_command_buffer_alloc_info(VkCommandPool cmd_pool, uint32 count)
    -> VkCommandBufferAllocateInfo;

[[nodiscard]] auto alloc_single_submit_command_buffer(VkDevice device,
                                                      VkCommandPool cmd_pool,
                                                      VkResult* result = nullptr)
    -> VkCommandBuffer;

auto execute_single_submit_commands(const CommandContext& ctx, VkCommandBuffer cmd_buffer)
    -> VkResult;

/**
 * Records commands into a command buffer, submits it, and waits for it to complete.
 *
 * \param device   the command context.
 * \param callback the function object used to record commands.
 *
 * \return the resulting error code.
 */
auto execute_now(const CommandContext& ctx, const CommandBufferCallback& callback)
    -> VkResult;

}  // namespace grace
