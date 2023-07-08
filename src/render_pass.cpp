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

#include "grace/render_pass.hpp"

#include <cassert>  // assert

namespace grace {

auto make_attachment_description(const VkFormat format,
                                 const VkImageLayout initial_layout,
                                 const VkImageLayout final_layout,
                                 const VkSampleCountFlagBits samples)
    -> VkAttachmentDescription
{
  return {
      .flags = 0,
      .format = format,
      .samples = samples,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = initial_layout,
      .finalLayout = final_layout,
  };
}

auto make_subpass_dependency(const uint32 src_subpass,
                             const uint32 dst_subpass,
                             const VkPipelineStageFlags src_stages,
                             const VkPipelineStageFlags dst_stages,
                             const VkAccessFlags src_access,
                             const VkAccessFlags dst_access) -> VkSubpassDependency
{
  return {
      .srcSubpass = src_subpass,
      .dstSubpass = dst_subpass,
      .srcStageMask = src_stages,
      .dstStageMask = dst_stages,
      .srcAccessMask = src_access,
      .dstAccessMask = dst_access,
      .dependencyFlags = 0,
  };
}

auto make_attachment_reference(const uint32 attachment, const VkImageLayout layout)
    -> VkAttachmentReference
{
  return {
      .attachment = attachment,
      .layout = layout,
  };
}

auto make_render_pass_info(const std::vector<VkAttachmentDescription>& attachments,
                           const std::vector<VkSubpassDescription>& subpasses,
                           const std::vector<VkSubpassDependency>& dependencies)
    -> VkRenderPassCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = u32_size(attachments),
      .pAttachments = data_or_null(attachments),
      .subpassCount = u32_size(subpasses),
      .pSubpasses = data_or_null(subpasses),
      .dependencyCount = u32_size(dependencies),
      .pDependencies = data_or_null(dependencies),
  };
}

auto make_render_pass_begin_info(VkRenderPass render_pass,
                                 VkFramebuffer framebuffer,
                                 const VkRect2D& render_area,
                                 const VkClearValue* clear_values,
                                 const uint32 clear_value_count) -> VkRenderPassBeginInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = render_pass,
      .framebuffer = framebuffer,
      .renderArea = render_area,
      .clearValueCount = clear_value_count,
      .pClearValues = clear_values,
  };
}

RenderPass::RenderPass(VkDevice device, VkRenderPass render_pass) noexcept
    : mDevice {device},
      mRenderPass {render_pass}
{
}

RenderPass::RenderPass(RenderPass&& other) noexcept
    : mDevice {other.mDevice},
      mRenderPass {other.mRenderPass}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mRenderPass = VK_NULL_HANDLE;
}

auto RenderPass::operator=(RenderPass&& other) noexcept -> RenderPass&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mRenderPass = other.mRenderPass;

    other.mDevice = VK_NULL_HANDLE;
    other.mRenderPass = VK_NULL_HANDLE;
  }

  return *this;
}

RenderPass::~RenderPass() noexcept
{
  destroy();
}

void RenderPass::destroy() noexcept
{
  if (mRenderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    mRenderPass = VK_NULL_HANDLE;
  }
}

auto RenderPass::make(VkDevice device,
                      const VkRenderPassCreateInfo& render_pass_info,
                      VkResult* result) -> RenderPass
{
  VkRenderPass render_pass_handle = VK_NULL_HANDLE;
  const auto status =
      vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass_handle);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return RenderPass {device, render_pass_handle};
  }

  return {};
}

auto RenderPass::make(VkDevice device,
                      const std::vector<VkAttachmentDescription>& attachments,
                      const std::vector<VkSubpassDescription>& subpasses,
                      const std::vector<VkSubpassDependency>& dependencies,
                      VkResult* result) -> RenderPass
{
  const auto render_pass_info =
      make_render_pass_info(attachments, subpasses, dependencies);
  return RenderPass::make(device, render_pass_info, result);
}

RenderPassBuilder::RenderPassBuilder(VkDevice device)
    : mDevice {device}
{
}

auto RenderPassBuilder::reset() -> Self&
{
  mAttachments.clear();
  mSubpassDependencies.clear();
  mSubpasses.clear();
  mActiveSubpassIndex.reset();

  return *this;
}

auto RenderPassBuilder::attachment(const VkFormat format,
                                   const VkImageLayout initial_layout,
                                   const VkImageLayout final_layout,
                                   const VkSampleCountFlagBits samples) -> Self&
{
  return attachment(
      make_attachment_description(format, initial_layout, final_layout, samples));
}

auto RenderPassBuilder::attachment(const VkAttachmentDescription& description) -> Self&
{
  mAttachments.push_back(description);
  return *this;
}

auto RenderPassBuilder::color_attachment(const VkFormat format,
                                         const VkImageLayout initial_layout,
                                         const VkSampleCountFlagBits samples) -> Self&
{
  return attachment(format, initial_layout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, samples);
}

auto RenderPassBuilder::depth_attachment(const VkFormat format,
                                         const VkImageLayout initial_layout,
                                         const VkSampleCountFlagBits samples) -> Self&
{
  return attachment(format,
                    initial_layout,
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    samples);
}

auto RenderPassBuilder::subpass_dependency(const VkSubpassDependency& dependency) -> Self&
{
  mSubpassDependencies.push_back(dependency);
  return *this;
}

auto RenderPassBuilder::subpass_dependency(const uint32 src_subpass,
                                           const uint32 dst_subpass,
                                           const VkPipelineStageFlags src_stages,
                                           const VkPipelineStageFlags dst_stages,
                                           const VkAccessFlags src_access,
                                           const VkAccessFlags dst_access) -> Self&
{
  return subpass_dependency(make_subpass_dependency(src_subpass,
                                                    dst_subpass,
                                                    src_stages,
                                                    dst_stages,
                                                    src_access,
                                                    dst_access));
}

auto RenderPassBuilder::begin_subpass(const VkPipelineBindPoint bind_point) -> Self&
{
  assert(!mActiveSubpassIndex.has_value() && "Previous subpass was not ended");

  auto& subpass = mSubpasses.emplace_back();
  subpass.pipeline_bind_point = bind_point;

  mActiveSubpassIndex = mSubpasses.size() - 1;

  return *this;
}

auto RenderPassBuilder::use_color_attachment(const uint32 attachment) -> Self&
{
  assert(mActiveSubpassIndex.has_value() && "Missing call to begin_subpass");

  auto& subpass = mSubpasses.at(mActiveSubpassIndex.value());
  subpass.color_attachments.push_back(
      make_attachment_reference(attachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));

  return *this;
}

auto RenderPassBuilder::use_input_attachment(const uint32 attachment) -> Self&
{
  assert(mActiveSubpassIndex.has_value() && "Missing call to begin_subpass");

  auto& subpass = mSubpasses.at(mActiveSubpassIndex.value());
  subpass.input_attachments.push_back(
      make_attachment_reference(attachment, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL));

  return *this;
}

auto RenderPassBuilder::use_depth_attachment(const uint32 attachment) -> Self&
{
  assert(mActiveSubpassIndex.has_value() && "Missing call to begin_subpass");

  auto& subpass = mSubpasses.at(mActiveSubpassIndex.value());
  subpass.depth_attachment =
      make_attachment_reference(attachment,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  subpass.has_depth_attachment = true;

  return *this;
}

auto RenderPassBuilder::end_subpass() -> Self&
{
  assert(mActiveSubpassIndex.has_value() &&
         "end_subpass must be called after begin_subpass");
  mActiveSubpassIndex.reset();

  return *this;
}

auto RenderPassBuilder::build(VkResult* result) const -> RenderPass
{
  assert(mDevice != VK_NULL_HANDLE);
  assert(!mActiveSubpassIndex.has_value() && "Missing call to end_subpass");

  const auto subpass_descriptions = get_subpass_descriptions();
  const auto render_pass_info = get_render_pass_info(subpass_descriptions);

  return RenderPass::make(mDevice, render_pass_info, result);
}

auto RenderPassBuilder::get_subpass_descriptions() const
    -> std::vector<VkSubpassDescription>
{
  std::vector<VkSubpassDescription> subpass_descriptions;
  subpass_descriptions.reserve(mSubpasses.size());

  for (const auto& subpass : mSubpasses) {
    auto& subpass_desc = subpass_descriptions.emplace_back();
    subpass_desc.flags = 0;
    subpass_desc.pipelineBindPoint = subpass.pipeline_bind_point;
    subpass_desc.inputAttachmentCount = u32_size(subpass.input_attachments);
    subpass_desc.pInputAttachments = data_or_null(subpass.input_attachments);
    subpass_desc.colorAttachmentCount = u32_size(subpass.color_attachments);
    subpass_desc.pColorAttachments = data_or_null(subpass.color_attachments);
    subpass_desc.pResolveAttachments = nullptr;
    subpass_desc.preserveAttachmentCount = 0;
    subpass_desc.pPreserveAttachments = nullptr;
    subpass_desc.pDepthStencilAttachment = subpass.has_depth_attachment
                                               ? &subpass.depth_attachment  //
                                               : nullptr;
  }

  return subpass_descriptions;
}

auto RenderPassBuilder::get_render_pass_info(
    const std::vector<VkSubpassDescription>& subpass_descriptions) const
    -> VkRenderPassCreateInfo
{
  return make_render_pass_info(mAttachments, subpass_descriptions, mSubpassDependencies);
}

}  // namespace grace
