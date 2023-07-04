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

auto make_submit_info(VkCommandBuffer cmd_buffer,
                      VkSemaphore wait_sem,
                      const VkPipelineStageFlags wait_dst_stage_mask,
                      VkSemaphore signal_sem) -> VkSubmitInfo
{
  VkSubmitInfo submit_info = {};

  const uint32 wait_sem_count = (wait_sem != VK_NULL_HANDLE) ? 1 : 0;
  const uint32 signal_sem_count = (signal_sem != VK_NULL_HANDLE) ? 1 : 0;
  const auto* wait_dst_stage_mask_ptr =
      (wait_dst_stage_mask != 0) ? &wait_dst_stage_mask : nullptr;

  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  submit_info.waitSemaphoreCount = wait_sem_count;
  submit_info.pWaitSemaphores = &wait_sem;
  submit_info.pWaitDstStageMask = wait_dst_stage_mask_ptr;

  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd_buffer;

  submit_info.signalSemaphoreCount = signal_sem_count;
  submit_info.pSignalSemaphores = &signal_sem;

  return submit_info;
}

}  // namespace grace
