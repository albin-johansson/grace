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
