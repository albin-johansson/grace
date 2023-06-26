#pragma once

#include <vulkan/vulkan.h>

#include "grace/common.hpp"

namespace grace {

[[nodiscard]] auto make_submit_info(VkCommandBuffer cmd_buffer,
                                    VkSemaphore wait_sem = VK_NULL_HANDLE,
                                    VkPipelineStageFlags wait_dst_stage_mask = 0,
                                    VkSemaphore signal_sem = VK_NULL_HANDLE)
    -> VkSubmitInfo;

}  // namespace grace
