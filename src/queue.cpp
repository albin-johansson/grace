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

#include "grace/queue.hpp"

namespace grace {

auto make_submit_info(const VkCommandBuffer* cmd_buffers,
                      const uint32 cmd_buffer_count,
                      const VkSemaphore* wait_semaphores,
                      const uint32 wait_semaphore_count,
                      const VkPipelineStageFlags* wait_dst_stage_mask,
                      const VkSemaphore* signal_semaphores,
                      const uint32 signal_semaphore_count) -> VkSubmitInfo
{
  VkSubmitInfo submit_info = {};

  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = wait_semaphore_count;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_dst_stage_mask;
  submit_info.commandBufferCount = cmd_buffer_count;
  submit_info.pCommandBuffers = cmd_buffers;
  submit_info.signalSemaphoreCount = signal_semaphore_count;
  submit_info.pSignalSemaphores = signal_semaphores;

  return submit_info;
}

auto make_present_info(const VkSemaphore* wait_semaphores,
                       const uint32 wait_semaphore_count,
                       const VkSwapchainKHR* swapchains,
                       const uint32 swapchain_count,
                       const uint32* swapchain_image_indices) -> VkPresentInfoKHR
{
  return {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = wait_semaphore_count,
      .pWaitSemaphores = wait_semaphores,
      .swapchainCount = swapchain_count,
      .pSwapchains = swapchains,
      .pImageIndices = swapchain_image_indices,
      .pResults = nullptr,
  };
}

}  // namespace grace
