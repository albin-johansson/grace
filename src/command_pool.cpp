#include "grace/command_pool.hpp"

#include "grace/queue.hpp"

namespace grace {

auto make_command_pool_info(const uint32 queue_family_index,
                            const VkCommandPoolCreateFlags flags)
    -> VkCommandPoolCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
      .queueFamilyIndex = queue_family_index,
  };
}

auto make_command_buffer_alloc_info(VkCommandPool cmd_pool, const uint32 count)
    -> VkCommandBufferAllocateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = cmd_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = count,
  };
}

auto alloc_single_submit_command_buffer(VkDevice device,
                                        VkCommandPool cmd_pool,
                                        VkResult* result) -> VkCommandBuffer
{
  const auto alloc_info = make_command_buffer_alloc_info(cmd_pool, 1);

  VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;
  const auto status = vkAllocateCommandBuffers(device, &alloc_info, &cmd_buffer);

  if (result) {
    *result = status;
  }

  return cmd_buffer;
}

auto execute_single_submit_commands(const CommandContext& ctx, VkCommandBuffer cmd_buffer)
    -> VkResult
{
  auto result = vkEndCommandBuffer(cmd_buffer);
  if (result != VK_SUCCESS) {
    return result;
  }

  const auto submit_info = make_submit_info(cmd_buffer);
  result = vkQueueSubmit(ctx.queue, 1, &submit_info, VK_NULL_HANDLE);
  if (result != VK_SUCCESS) {
    return result;
  }

  result = vkQueueWaitIdle(ctx.queue);
  if (result != VK_SUCCESS) {
    return result;
  }

  vkFreeCommandBuffers(ctx.device, ctx.cmd_pool, 1, &cmd_buffer);
  return VK_SUCCESS;
}

auto execute_now(const CommandContext& ctx, const CommandBufferCallback& callback)
    -> VkResult
{
  VkResult result = VK_SUCCESS;
  VkCommandBuffer cmd_buffer =
      alloc_single_submit_command_buffer(ctx.device, ctx.cmd_pool, &result);

  if (cmd_buffer) {
    callback(cmd_buffer);
    result = execute_single_submit_commands(ctx, cmd_buffer);
  }

  return result;
}

}  // namespace grace