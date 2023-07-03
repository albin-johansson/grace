#pragma once

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_framebuffer_info(VkRenderPass render_pass,
                                         const VkExtent2D& extent,
                                         const VkImageView* attachments,
                                         uint32 attachment_count)
    -> VkFramebufferCreateInfo;

class Framebuffer final {
 public:
  Framebuffer() noexcept = default;

  Framebuffer(Framebuffer&& other) noexcept;
  Framebuffer(const Framebuffer& other) = delete;

  Framebuffer& operator=(Framebuffer&& other) noexcept;
  Framebuffer& operator=(const Framebuffer& other) = delete;

  ~Framebuffer() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkFramebufferCreateInfo& fb_info,
                                 VkResult* result = nullptr) -> Framebuffer;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkRenderPass render_pass,
                                 VkImageView color_buffer,
                                 VkImageView depth_buffer,
                                 const VkExtent2D& extent,
                                 VkResult* result = nullptr) -> Framebuffer;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkFramebuffer { return mFramebuffer; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mFramebuffer != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkFramebuffer mFramebuffer {VK_NULL_HANDLE};
};

}  // namespace grace
