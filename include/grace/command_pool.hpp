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

#pragma once

#include <functional>  // function
#include <vector>      // vector

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

[[nodiscard]] auto make_command_buffer_begin_info(
    const VkCommandBufferInheritanceInfo* inheritance = nullptr,
    VkCommandBufferUsageFlags flags = 0) -> VkCommandBufferBeginInfo;

[[nodiscard]] auto alloc_single_submit_command_buffer(VkDevice device,
                                                      VkCommandPool cmd_pool,
                                                      VkResult* result = nullptr)
    -> VkCommandBuffer;

[[nodiscard]] auto alloc_command_buffer(VkDevice device,
                                        VkCommandPool cmd_pool,
                                        VkResult* result = nullptr) -> VkCommandBuffer;

[[nodiscard]] auto alloc_command_buffers(VkDevice device,
                                         VkCommandPool cmd_pool,
                                         uint32 count,
                                         VkResult* result = nullptr)
    -> std::vector<VkCommandBuffer>;

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

class CommandPool final {
 public:
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkCommandPoolCreateInfo& pool_info,
                                 VkResult* result = nullptr) -> CommandPool;

  [[nodiscard]] static auto make(VkDevice device,
                                 uint32 queue_family_index,
                                 VkCommandPoolCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> CommandPool;

  CommandPool() noexcept = default;

  CommandPool(VkDevice device, VkCommandPool command_pool) noexcept;

  CommandPool(CommandPool&& other) noexcept;
  CommandPool(const CommandPool& other) = delete;

  auto operator=(CommandPool&& other) noexcept -> CommandPool&;
  auto operator=(const CommandPool& other) -> CommandPool& = delete;

  ~CommandPool() noexcept;

  void destroy() noexcept;

  auto execute_now(VkQueue queue, const CommandBufferCallback& callback) -> VkResult;

  [[nodiscard]] auto alloc_single_submit_command_buffer(VkResult* result = nullptr)
      -> VkCommandBuffer;

  [[nodiscard]] auto get() noexcept -> VkCommandPool { return mCommandPool; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkCommandPool() noexcept { return mCommandPool; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mCommandPool != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkCommandPool mCommandPool {VK_NULL_HANDLE};
};

}  // namespace grace
