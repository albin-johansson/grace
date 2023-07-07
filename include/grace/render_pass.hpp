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

#include <optional>  // optional
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

/**
 * Creates a render pass attachment description object.
 *
 * \details This function uses the following defaults:\n
 * <ul>
 *   <li>The flags are zeroed.</li>
 *   <li>The load operation is set to `VK_ATTACHMENT_LOAD_OP_CLEAR`.</li>
 *   <li>The store operation is set to `VK_ATTACHMENT_STORE_OP_STORE`.</li>
 *   <li>The stencil load operation is set to `VK_ATTACHMENT_LOAD_OP_DONT_CARE`.</li>
 *   <li>The stencil store operation is set to `VK_ATTACHMENT_STORE_OP_DONT_CARE`.</li>
 * </ul>
 *
 * \param format         the format of the image view used for the attachment.
 * \param initial_layout the layout of the attachment image when the render pass begins.
 * \param final_layout   the layout of the attachment image after the render pass ends.
 * \param samples        the number of samples per texel.
 *
 * \return a render pass attachment specification.
 */
[[nodiscard]] auto make_attachment_description(
    VkFormat format,
    VkImageLayout initial_layout,
    VkImageLayout final_layout,
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) -> VkAttachmentDescription;

/**
 * Creates a subpass dependency specification.
 *
 * \param src_subpass the index of the source subpass, or `VK_SUBPASS_EXTERNAL`.
 * \param dst_subpass the index of the destination subpass, or `VK_SUBPASS_EXTERNAL`.
 * \param src_stages  a bitmask of the source stages.
 * \param dst_stages  a bitmask of the destination stages.
 * \param src_access  a bitmask of the source access.
 * \param dst_access  a bitmask of the destination access.
 *
 * \return a subpass dependency specification.
 */
[[nodiscard]] auto make_subpass_dependency(uint32 src_subpass,
                                           uint32 dst_subpass,
                                           VkPipelineStageFlags src_stages,
                                           VkPipelineStageFlags dst_stages,
                                           VkAccessFlags src_access = 0,
                                           VkAccessFlags dst_access = 0)
    -> VkSubpassDependency;

[[nodiscard]] auto make_attachment_reference(uint32 attachment, VkImageLayout layout)
    -> VkAttachmentReference;

[[nodiscard]] auto make_render_pass_info(
    const std::vector<VkAttachmentDescription>& attachments,
    const std::vector<VkSubpassDescription>& subpasses,
    const std::vector<VkSubpassDependency>& dependencies) -> VkRenderPassCreateInfo;

[[nodiscard]] auto make_render_pass_begin_info(VkRenderPass render_pass,
                                               VkFramebuffer framebuffer,
                                               const VkRect2D& render_area,
                                               const VkClearValue* clear_values,
                                               uint32 clear_value_count)
    -> VkRenderPassBeginInfo;

class RenderPass final {
 public:
  RenderPass() noexcept = default;

  RenderPass(VkDevice device, VkRenderPass render_pass) noexcept;

  RenderPass(RenderPass&& other) noexcept;
  RenderPass(const RenderPass& other) = delete;

  auto operator=(RenderPass&& other) noexcept -> RenderPass&;
  auto operator=(const RenderPass& other) -> RenderPass& = delete;

  ~RenderPass() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkRenderPassCreateInfo& render_pass_info,
                                 VkResult* result = nullptr) -> RenderPass;

  [[nodiscard]] static auto make(VkDevice device,
                                 const std::vector<VkAttachmentDescription>& attachments,
                                 const std::vector<VkSubpassDescription>& subpasses,
                                 const std::vector<VkSubpassDependency>& dependencies,
                                 VkResult* result = nullptr) -> RenderPass;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkRenderPass { return mRenderPass; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkRenderPass() noexcept { return mRenderPass; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mRenderPass != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkRenderPass mRenderPass {VK_NULL_HANDLE};
};

class RenderPassBuilder final {
 public:
  using Self = RenderPassBuilder;

  explicit RenderPassBuilder(VkDevice device);

  /**
   * Resets the internal state.
   *
   * \details This function is only necessary if you want to use the builder to create
   *          multiple render pass instances.
   *
   * \return the render pass builder itself.
   */
  auto reset() -> Self&;

  /**
   * Adds an attachment to the render pass (independent of the current subpass).
   *
   * \param description the attachment specification.
   *
   * \return the render pass builder itself.
   */
  auto attachment(const VkAttachmentDescription& description) -> Self&;

  /**
   * Adds an attachment to the render pass (independent of the current subpass).
   *
   * \param format         the format of the image view used for the attachment.
   * \param initial_layout the layout of the attachment image when the render pass begins.
   * \param final_layout   the layout of the attachment image after the render pass ends.
   * \param samples        the number of samples per texel.
   *
   * \return the render pass builder itself.
   */
  auto attachment(VkFormat format,
                  VkImageLayout initial_layout,
                  VkImageLayout final_layout,
                  VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) -> Self&;

  /**
   * Adds a color attachment to the render pass (independent of the current subpass).
   *
   * \details This is a convenience function that assumes a final layout of
   *          `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR`.
   *
   * \param format         the format of the image view used for the attachment.
   * \param initial_layout the layout of the attachment image when the render pass begins.
   * \param samples        the number of samples per texel.
   *
   * \return the render pass builder itself.
   */
  auto color_attachment(VkFormat format,
                        VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED,
                        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) -> Self&;

  /**
   * Adds a depth attachment to the render pass (independent of the current subpass).
   *
   * \details This is a convenience function that assumes a final layout of
   *          `VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL`.
   *
   * \param format         the format of the image view used for the attachment.
   * \param initial_layout the layout of the attachment image when the render pass begins.
   * \param samples        the number of samples per texel.
   *
   * \return the render pass builder itself.
   */
  auto depth_attachment(VkFormat format,
                        VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED,
                        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) -> Self&;

  /**
   * Registers a subpass dependency.
   *
   * \param dependency the subpass dependency specification.
   *
   * \return the render pass builder itself.
   */
  auto subpass_dependency(const VkSubpassDependency& dependency) -> Self&;

  /**
   * Registers a subpass dependency.
   *
   * \param src_subpass the index of the source subpass, or `VK_SUBPASS_EXTERNAL`.
   * \param dst_subpass the index of the destination subpass, or `VK_SUBPASS_EXTERNAL`.
   * \param src_stages  a bitmask of the source stages.
   * \param dst_stages  a bitmask of the destination stages.
   * \param src_access  a bitmask of the source access.
   * \param dst_access  a bitmask of the destination access.
   *
   * \return the render pass builder itself.
   */
  auto subpass_dependency(uint32 src_subpass,
                          uint32 dst_subpass,
                          VkPipelineStageFlags src_stages,
                          VkPipelineStageFlags dst_stages,
                          VkAccessFlags src_access = 0,
                          VkAccessFlags dst_access = 0) -> Self&;

  /**
   * Begin describing a new subpass.
   *
   * \pre Calls to this function must be interleaved with calls to `end_subpass`.
   *
   * \param bind_point the pipeline bind point associated with the subpass.
   *
   * \return the render pass builder itself.
   */
  auto begin_subpass(VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS)
      -> Self&;

  /**
   * Marks the attachment at the specified index as a color attachment in the subpass.
   *
   * \pre This function must be called between `begin_subpass`/`end_subpass` invocations.
   *
   * \param attachment the attachment index.
   *
   * \return the render pass builder itself.
   */
  auto set_color_attachment(uint32 attachment) -> Self&;

  /**
   * Marks the attachment at the specified index as an input attachment in the subpass.
   *
   * \pre This function must be called between `begin_subpass`/`end_subpass` invocations.
   *
   * \param attachment the attachment index.
   *
   * \return the render pass builder itself.
   */
  auto set_input_attachment(uint32 attachment) -> Self&;

  /**
   * Marks the attachment at the specified index as a depth attachment in the subpass.
   *
   * \pre This function must be called between `begin_subpass`/`end_subpass` invocations.
   * \pre Only one attachment can be marked as the depth attachment.
   *
   * \param attachment the attachment index.
   *
   * \return the render pass builder itself.
   */
  auto set_depth_attachment(uint32 attachment) -> Self&;

  /**
   * Marks the end of the current subpass.
   *
   * \pre This function must be called after a corresponding `begin_subpass` invocation.
   *
   * \return the render pass builder itself.
   */
  auto end_subpass() -> Self&;

  /**
   * Attempts to create the specified render pass.
   *
   * \param[out] result the resulting error code.
   *
   * \return a potentially null render pass.
   */
  [[nodiscard]] auto build(VkResult* result = nullptr) const -> RenderPass;

 private:
  struct SubpassInfo final {
    std::vector<VkAttachmentReference> color_attachments;
    std::vector<VkAttachmentReference> input_attachments;
    VkAttachmentReference depth_attachment {};
    VkSubpassDescription description {};
    bool has_depth_attachment {false};
  };

  VkDevice mDevice {VK_NULL_HANDLE};
  std::vector<VkAttachmentDescription> mAttachments;
  std::vector<VkSubpassDependency> mSubpassDependencies;
  std::vector<SubpassInfo> mSubpasses;
  std::optional<usize> mActiveSubpassIndex;
  std::optional<uint32> mDepthAttachmentIndex;
};

}  // namespace grace
