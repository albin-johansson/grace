/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

auto alloc_command_buffer(VkDevice device, VkCommandPool cmd_pool, VkResult* result)
    -> VkCommandBuffer
{
  const auto alloc_info = make_command_buffer_alloc_info(cmd_pool, 1);

  VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;
  const auto status = vkAllocateCommandBuffers(device, &alloc_info, &cmd_buffer);

  if (result) {
    *result = status;
  }

  return cmd_buffer;
}

auto alloc_command_buffers(VkDevice device,
                           VkCommandPool cmd_pool,
                           const uint32 count,
                           VkResult* result) -> std::vector<VkCommandBuffer>
{
  const auto alloc_info = make_command_buffer_alloc_info(cmd_pool, count);

  std::vector<VkCommandBuffer> cmd_buffers;
  cmd_buffers.resize(static_cast<usize>(count));

  const auto status = vkAllocateCommandBuffers(device, &alloc_info, cmd_buffers.data());

  if (result) {
    *result = status;
  }

  return cmd_buffers;
}

auto execute_single_submit_commands(const CommandContext& ctx, VkCommandBuffer cmd_buffer)
    -> VkResult
{
  auto result = vkEndCommandBuffer(cmd_buffer);
  if (result != VK_SUCCESS) {
    return result;
  }

  const auto submit_info = make_submit_info(&cmd_buffer, 1);
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

CommandPool::CommandPool(VkDevice device, VkCommandPool command_pool) noexcept
    : mDevice {device},
      mCommandPool {command_pool}
{
}

CommandPool::CommandPool(CommandPool&& other) noexcept
    : mDevice {other.mDevice},
      mCommandPool {other.mCommandPool}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mCommandPool = VK_NULL_HANDLE;
}

auto CommandPool::operator=(CommandPool&& other) noexcept -> CommandPool&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mCommandPool = other.mCommandPool;

    other.mDevice = VK_NULL_HANDLE;
    other.mCommandPool = VK_NULL_HANDLE;
  }

  return *this;
}

CommandPool::~CommandPool() noexcept
{
  destroy();
}

void CommandPool::destroy() noexcept
{
  if (mCommandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    mCommandPool = VK_NULL_HANDLE;
  }
}

auto CommandPool::execute_now(VkQueue queue, const CommandBufferCallback& callback)
    -> VkResult
{
  CommandContext ctx;
  ctx.device = mDevice;
  ctx.cmd_pool = mCommandPool;
  ctx.queue = queue;
  return grace::execute_now(ctx, callback);
}

auto CommandPool::alloc_single_submit_command_buffer(VkResult* result) -> VkCommandBuffer
{
  return grace::alloc_single_submit_command_buffer(mDevice, mCommandPool, result);
}

auto CommandPool::make(VkDevice device,
                       const VkCommandPoolCreateInfo& pool_info,
                       VkResult* result) -> CommandPool
{
  VkCommandPool command_pool = VK_NULL_HANDLE;
  const auto status = vkCreateCommandPool(device, &pool_info, nullptr, &command_pool);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return CommandPool {device, command_pool};
  }

  return {};
}

auto CommandPool::make(VkDevice device,
                       const uint32 queue_family_index,
                       const VkCommandPoolCreateFlags flags,
                       VkResult* result) -> CommandPool
{
  const auto pool_info = make_command_pool_info(queue_family_index, flags);
  return CommandPool::make(device, pool_info, result);
}

}  // namespace grace