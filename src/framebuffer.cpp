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

#include "grace/framebuffer.hpp"

namespace grace {

auto make_framebuffer_info(VkRenderPass render_pass,
                           const VkExtent2D& extent,
                           const VkImageView* attachments,
                           const uint32 attachment_count) -> VkFramebufferCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = render_pass,
      .attachmentCount = attachment_count,
      .pAttachments = attachments,
      .width = extent.width,
      .height = extent.height,
      .layers = 1,
  };
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : mDevice {other.mDevice},
      mFramebuffer {other.mFramebuffer}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mFramebuffer = VK_NULL_HANDLE;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mFramebuffer = other.mFramebuffer;

    other.mDevice = VK_NULL_HANDLE;
    other.mFramebuffer = VK_NULL_HANDLE;
  }

  return *this;
}

Framebuffer::~Framebuffer() noexcept
{
  destroy();
}

void Framebuffer::destroy() noexcept
{
  if (mFramebuffer != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
    mFramebuffer = VK_NULL_HANDLE;
  }
}

auto Framebuffer::make(VkDevice device,
                       const VkFramebufferCreateInfo& fb_info,
                       VkResult* result) -> Framebuffer
{
  Framebuffer framebuffer;
  framebuffer.mDevice = device;

  const auto status =
      vkCreateFramebuffer(device, &fb_info, nullptr, &framebuffer.mFramebuffer);

  if (result) {
    *result = status;
  }

  if (status != VK_SUCCESS) {
    return {};
  }

  return framebuffer;
}

auto Framebuffer::make(VkDevice device,
                       VkRenderPass render_pass,
                       VkImageView color_buffer,
                       VkImageView depth_buffer,
                       const VkExtent2D& extent,
                       VkResult* result) -> Framebuffer
{
  const VkImageView attachments[] = {color_buffer, depth_buffer};
  const auto fb_info = make_framebuffer_info(render_pass,
                                             extent,
                                             attachments,
                                             (depth_buffer != VK_NULL_HANDLE) ? 2 : 1);
  return Framebuffer::make(device, fb_info, result);
}

}  // namespace grace
