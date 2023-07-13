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

  auto operator=(Framebuffer&& other) noexcept -> Framebuffer&;
  auto operator=(const Framebuffer& other) -> Framebuffer& = delete;

  ~Framebuffer() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkFramebufferCreateInfo& fb_info,
                                 VkResult* result = nullptr) -> Framebuffer;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkRenderPass render_pass,
                                 const VkImageView* attachments,
                                 uint32 attachment_count,
                                 const VkExtent2D& extent,
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
