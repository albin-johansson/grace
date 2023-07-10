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

#include "grace/pipeline.hpp"

#include <algorithm>  // find
#include <cstring>    // memcpy

#include "grace/shader_module.hpp"

namespace grace {

auto make_viewport(const float x,
                   const float y,
                   const float width,
                   const float height,
                   const float min_depth,
                   const float max_depth) -> VkViewport
{
  return {
      .x = x,
      .y = y,
      .width = width,
      .height = height,
      .minDepth = min_depth,
      .maxDepth = max_depth,
  };
}

auto make_rect_2d(const int32 x, const int32 y, const uint32 width, const uint32 height)
    -> VkRect2D
{
  return {
      .offset = {x, y},
      .extent = {width, height},
  };
}

auto make_pipeline_shader_stage_info(const VkShaderStageFlagBits stage,
                                     VkShaderModule shader_module,
                                     const VkSpecializationInfo* specialization,
                                     const char* entry_point)
    -> VkPipelineShaderStageCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = stage,
      .module = shader_module,
      .pName = entry_point,
      .pSpecializationInfo = specialization,
  };
}

auto make_pipeline_vertex_input_state_info(
    const std::vector<VkVertexInputBindingDescription>& bindings,
    const std::vector<VkVertexInputAttributeDescription>& attributes)
    -> VkPipelineVertexInputStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .vertexBindingDescriptionCount = u32_size(bindings),
      .pVertexBindingDescriptions = data_or_null(bindings),
      .vertexAttributeDescriptionCount = u32_size(attributes),
      .pVertexAttributeDescriptions = data_or_null(attributes),
  };
}

auto make_pipeline_input_assembly_state_info(const VkPrimitiveTopology topology)
    -> VkPipelineInputAssemblyStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = topology,
      .primitiveRestartEnable = VK_FALSE,
  };
}

auto make_pipeline_tessellation_state_info(const uint32 patch_control_points)
    -> VkPipelineTessellationStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .patchControlPoints = patch_control_points,
  };
}

auto make_pipeline_viewport_state_info(const std::vector<VkViewport>& viewports,
                                       const std::vector<VkRect2D>& scissors)
    -> VkPipelineViewportStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .viewportCount = u32_size(viewports),
      .pViewports = data_or_null(viewports),
      .scissorCount = u32_size(scissors),
      .pScissors = data_or_null(scissors),
  };
}

auto make_pipeline_color_blend_state_info(
    const bool op_enabled,
    const VkLogicOp op,
    const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
    const std::array<float, 4>& blend_constants) -> VkPipelineColorBlendStateCreateInfo
{
  VkPipelineColorBlendStateCreateInfo info = {};

  info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.logicOpEnable = op_enabled ? VK_TRUE : VK_FALSE;
  info.logicOp = op;
  info.attachmentCount = u32_size(attachments);
  info.pAttachments = data_or_null(attachments);

  std::memcpy(info.blendConstants, blend_constants.data(), sizeof blend_constants);

  return info;
}

auto make_pipeline_dynamic_state_info(const std::vector<VkDynamicState>& states)
    -> VkPipelineDynamicStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .dynamicStateCount = u32_size(states),
      .pDynamicStates = data_or_null(states),
  };
}

Pipeline::Pipeline(VkDevice device, VkPipeline pipeline) noexcept
    : mDevice {device},
      mPipeline {pipeline}
{
}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : mDevice {other.mDevice},
      mPipeline {other.mPipeline}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mPipeline = VK_NULL_HANDLE;
}

auto Pipeline::operator=(Pipeline&& other) noexcept -> Pipeline&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mPipeline = other.mPipeline;

    other.mDevice = VK_NULL_HANDLE;
    other.mPipeline = VK_NULL_HANDLE;
  }

  return *this;
}

Pipeline::~Pipeline() noexcept
{
  destroy();
}

void Pipeline::destroy() noexcept
{
  if (mPipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    mPipeline = VK_NULL_HANDLE;
  }
}

auto GraphicsPipeline::make(VkDevice device,
                            const VkGraphicsPipelineCreateInfo& pipeline_info,
                            VkPipelineCache cache,
                            VkResult* result) -> GraphicsPipeline
{
  VkPipeline pipeline = VK_NULL_HANDLE;
  const auto status =
      vkCreateGraphicsPipelines(device, cache, 1, &pipeline_info, nullptr, &pipeline);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return GraphicsPipeline {device, pipeline};
  }

  return {};
}

GraphicsPipelineBuilder::GraphicsPipelineBuilder(VkDevice device)
    : mDevice {device}
{
  reset();
}

auto GraphicsPipelineBuilder::reset() -> Self&
{
  mVertexInputBindings.clear();
  mVertexAttributes.clear();
  mViewports.clear();
  mScissors.clear();
  mDynamicStates.clear();
  mColorBlendAttachments.clear();

  mTessellationPatchControlPoints.reset();

  return with_layout(VK_NULL_HANDLE)
      .with_cache(VK_NULL_HANDLE)
      .with_render_pass(VK_NULL_HANDLE, 0)
      .vertex_shader(nullptr)
      .fragment_shader(nullptr)
      .primitive_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
      .rasterization(VK_POLYGON_MODE_FILL)
      .line_width(1.0f)
      .blend_constants(0, 0, 0, 0)
      .depth_bias(false)
      .depth_test(false)
      .depth_write(false)
      .depth_bounds_test(false)
      .depth_clamp(false)
      .stencil_test(false)
      .color_logic_op(false);
}

auto GraphicsPipelineBuilder::with_layout(VkPipelineLayout layout) -> Self&
{
  mLayout = layout;
  return *this;
}

auto GraphicsPipelineBuilder::with_cache(VkPipelineCache cache) -> Self&
{
  mCache = cache;
  return *this;
}

auto GraphicsPipelineBuilder::with_render_pass(VkRenderPass render_pass,
                                               const uint32 subpass) -> Self&
{
  mRenderPass = render_pass;
  mSubpass = subpass;
  return *this;
}

auto GraphicsPipelineBuilder::vertex_shader(const char* shader_path) -> Self&
{
  mVertexShaderPath = shader_path ? shader_path : std::string {};
  return *this;
}

auto GraphicsPipelineBuilder::fragment_shader(const char* shader_path) -> Self&
{
  mFragmentShaderPath = shader_path ? shader_path : std::string {};
  return *this;
}

auto GraphicsPipelineBuilder::vertex_input_binding(const uint32 binding,
                                                   const uint32 stride,
                                                   const VkVertexInputRate rate) -> Self&
{
  mVertexInputBindings.push_back({
      .binding = binding,
      .stride = stride,
      .inputRate = rate,
  });

  return *this;
}

auto GraphicsPipelineBuilder::vertex_attribute(const uint32 binding,
                                               const uint32 location,
                                               const VkFormat format,
                                               const uint32 offset) -> Self&
{
  mVertexAttributes.push_back({
      .location = location,
      .binding = binding,
      .format = format,
      .offset = offset,
  });

  return *this;
}

auto GraphicsPipelineBuilder::primitive_topology(const VkPrimitiveTopology topology)
    -> Self&
{
  mPrimitiveTopology = topology;
  return *this;
}

auto GraphicsPipelineBuilder::tessellation(const uint32 patch_control_points) -> Self&
{
  mTessellationPatchControlPoints = patch_control_points;
  return *this;
}

auto GraphicsPipelineBuilder::viewport(const float x,
                                       const float y,
                                       const float width,
                                       const float height,
                                       const float min_depth,
                                       const float max_depth) -> Self&
{
  mViewports.push_back(make_viewport(x, y, width, height, min_depth, max_depth));
  return *this;
}

auto GraphicsPipelineBuilder::scissor(const int32 x,
                                      const int32 y,
                                      const uint32 width,
                                      const uint32 height) -> Self&
{
  mScissors.push_back(make_rect_2d(x, y, width, height));
  return *this;
}

auto GraphicsPipelineBuilder::rasterization(const VkPolygonMode polygon_mode,
                                            const VkCullModeFlags cull_mode,
                                            const VkFrontFace front_face) -> Self&
{
  mPolygonMode = polygon_mode;
  mCullMode = cull_mode;
  mFrontFace = front_face;

  return *this;
}

auto GraphicsPipelineBuilder::color_logic_op(const bool enabled, const VkLogicOp op)
    -> Self&
{
  mColorLogicOpEnabled = enabled;
  mColorLogicOp = op;

  return *this;
}

auto GraphicsPipelineBuilder::blend_constants(const float red,
                                              const float green,
                                              const float blue,
                                              const float alpha) -> Self&
{
  mBlendConstants[0] = red;
  mBlendConstants[1] = green;
  mBlendConstants[2] = blue;
  mBlendConstants[3] = alpha;

  return *this;
}

auto GraphicsPipelineBuilder::color_blend_attachment(
    const VkPipelineColorBlendAttachmentState& blend_attachment) -> Self&
{
  mColorBlendAttachments.push_back(blend_attachment);
  return *this;
}

auto GraphicsPipelineBuilder::color_blend_attachment(const bool enabled,
                                                     const VkBlendOp op,
                                                     const VkBlendFactor src_factor,
                                                     const VkBlendFactor dst_factor,
                                                     const VkBlendFactor src_alpha_factor,
                                                     const VkBlendFactor dst_alpha_factor)
    -> Self&
{
  return color_blend_attachment(VkPipelineColorBlendAttachmentState {
      .blendEnable = enabled ? VK_TRUE : VK_FALSE,
      .srcColorBlendFactor = src_factor,
      .dstColorBlendFactor = dst_factor,
      .colorBlendOp = op,
      .srcAlphaBlendFactor = src_alpha_factor,
      .dstAlphaBlendFactor = dst_alpha_factor,
      .alphaBlendOp = op,
      .colorWriteMask = 0,
  });
}

auto GraphicsPipelineBuilder::line_width(const float width) -> Self&
{
  mLineWidth = width;
  return *this;
}

auto GraphicsPipelineBuilder::depth_bias(const bool enabled,
                                         const float constant_factor,
                                         const float slope_factor,
                                         const float clamp_value) -> Self&
{
  mDepthBiasEnabled = enabled;
  mDepthBiasConstantFactor = constant_factor;
  mDepthBiasSlopeFactor = slope_factor;
  mDepthBiasClampValue = clamp_value;

  return *this;
}

auto GraphicsPipelineBuilder::depth_test(const bool enabled, const VkCompareOp compare_op)
    -> Self&
{
  mDepthTestEnabled = enabled;
  mDepthCompareOp = compare_op;

  return *this;
}

auto GraphicsPipelineBuilder::depth_bounds_test(const bool enabled,
                                                const float min_depth,
                                                const float max_depth) -> Self&
{
  mDepthBoundsTestEnabled = enabled;
  mMinDepth = min_depth;
  mMaxDepth = max_depth;

  return *this;
}

auto GraphicsPipelineBuilder::depth_write(const bool enabled) -> Self&
{
  mDepthWriteEnabled = enabled;

  return *this;
}

auto GraphicsPipelineBuilder::depth_clamp(const bool enabled) -> Self&
{
  mDepthClampEnabled = enabled;

  return *this;
}

auto GraphicsPipelineBuilder::stencil_test(const bool enabled,
                                           const VkStencilOpState& front,
                                           const VkStencilOpState& back) -> Self&
{
  mStencilTestEnabled = enabled;
  mFrontStencilOpState = front;
  mBackStencilOpState = back;

  return *this;
}

auto GraphicsPipelineBuilder::dynamic_state(const VkDynamicState state) -> Self&
{
  mDynamicStates.push_back(state);
  return *this;
}

auto GraphicsPipelineBuilder::build(VkResult* result) const -> GraphicsPipeline
{
  if (!_is_complete()) {
    if (result) {
      *result = VK_INCOMPLETE;
    }

    return {};
  }

  auto vertex_shader = ShaderModule::read(mDevice, mVertexShaderPath.c_str(), result);
  if (!vertex_shader) {
    return {};
  }

  auto fragment_shader = ShaderModule::read(mDevice, mFragmentShaderPath.c_str(), result);
  if (!fragment_shader) {
    return {};
  }

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.flags = 0;
  pipeline_info.pNext = nullptr;

  // TODO shader customization (specializations, entry name)
  VkPipelineShaderStageCreateInfo shader_stages[2] = {};
  shader_stages[0] =
      make_pipeline_shader_stage_info(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader);
  shader_stages[1] =
      make_pipeline_shader_stage_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader);

  const auto vertex_input_state = get_vertex_input_state_info();
  const auto input_assembly_state = get_input_assembly_state_info();
  const auto tessellation_state = get_tessellation_state_info();
  const auto viewport_state = get_viewport_state_info();
  const auto rasterization_state = get_rasterization_state_info();
  const auto multisample_state = get_multisample_state_info();
  const auto depth_stencil_state = get_depth_stencil_state_info();
  const auto color_blend_state = get_color_blend_state_info();
  const auto dynamic_state = get_dynamic_state_info();

  pipeline_info.stageCount = u32_size(shader_stages);
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_state;
  pipeline_info.pInputAssemblyState = &input_assembly_state;
  pipeline_info.pTessellationState = nullptr;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterization_state;
  pipeline_info.pMultisampleState = &multisample_state;
  pipeline_info.pDepthStencilState = &depth_stencil_state;
  pipeline_info.pColorBlendState = &color_blend_state;
  pipeline_info.pDynamicState = nullptr;

  if (mTessellationPatchControlPoints.has_value()) {
    pipeline_info.pTessellationState = &tessellation_state;
  }

  if (!mDynamicStates.empty()) {
    pipeline_info.pDynamicState = &dynamic_state;
  }

  pipeline_info.layout = mLayout;
  pipeline_info.renderPass = mRenderPass;
  pipeline_info.subpass = mSubpass;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.basePipelineIndex = 0;

  return GraphicsPipeline::make(mDevice, pipeline_info, mCache, result);
}

auto GraphicsPipelineBuilder::_is_complete() const -> bool
{
  return mLayout != VK_NULL_HANDLE &&      //
         mRenderPass != VK_NULL_HANDLE &&  //
         !mVertexShaderPath.empty() &&     //
         !mFragmentShaderPath.empty();
}

auto GraphicsPipelineBuilder::get_vertex_input_state_info() const
    -> VkPipelineVertexInputStateCreateInfo
{
  return make_pipeline_vertex_input_state_info(mVertexInputBindings, mVertexAttributes);
}

auto GraphicsPipelineBuilder::get_input_assembly_state_info() const
    -> VkPipelineInputAssemblyStateCreateInfo
{
  return make_pipeline_input_assembly_state_info(mPrimitiveTopology);
}

auto GraphicsPipelineBuilder::get_tessellation_state_info() const
    -> VkPipelineTessellationStateCreateInfo
{
  if (mTessellationPatchControlPoints.has_value()) {
    return make_pipeline_tessellation_state_info(*mTessellationPatchControlPoints);
  }

  return make_pipeline_tessellation_state_info(0);
}

auto GraphicsPipelineBuilder::get_viewport_state_info() const
    -> VkPipelineViewportStateCreateInfo
{
  auto info = make_pipeline_viewport_state_info(mViewports, mScissors);

  if (std::find(mDynamicStates.begin(),
                mDynamicStates.end(),
                VK_DYNAMIC_STATE_VIEWPORT) != mDynamicStates.end()) {
    info.pViewports = nullptr;
    info.viewportCount = 1;
  }

  if (std::find(mDynamicStates.begin(), mDynamicStates.end(), VK_DYNAMIC_STATE_SCISSOR) !=
      mDynamicStates.end()) {
    info.pScissors = nullptr;
    info.scissorCount = 1;
  }

  return info;
}

auto GraphicsPipelineBuilder::get_rasterization_state_info() const
    -> VkPipelineRasterizationStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthClampEnable = mDepthClampEnabled ? VK_TRUE : VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = mPolygonMode,
      .cullMode = mCullMode,
      .frontFace = mFrontFace,
      .depthBiasEnable = mDepthBiasEnabled ? VK_TRUE : VK_FALSE,
      .depthBiasConstantFactor = mDepthBiasConstantFactor,
      .depthBiasClamp = mDepthBiasClampValue,
      .depthBiasSlopeFactor = mDepthBiasSlopeFactor,
      .lineWidth = mLineWidth,
  };
}

auto GraphicsPipelineBuilder::get_multisample_state_info() const
    -> VkPipelineMultisampleStateCreateInfo
{
  // TODO sample_shading(bool enabled, float min_sample_shading = 1.0f);
  // TODO sample_mask_test
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 0.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };
}

auto GraphicsPipelineBuilder::get_depth_stencil_state_info() const
    -> VkPipelineDepthStencilStateCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthTestEnable = mDepthTestEnabled,
      .depthWriteEnable = mDepthWriteEnabled ? VK_TRUE : VK_FALSE,
      .depthCompareOp = mDepthCompareOp,
      .depthBoundsTestEnable = mDepthBoundsTestEnabled ? VK_TRUE : VK_FALSE,
      .stencilTestEnable = mStencilTestEnabled,
      .front = mFrontStencilOpState,
      .back = mBackStencilOpState,
      .minDepthBounds = mMinDepth,
      .maxDepthBounds = mMaxDepth,
  };
}

auto GraphicsPipelineBuilder::get_color_blend_state_info() const
    -> VkPipelineColorBlendStateCreateInfo
{
  return make_pipeline_color_blend_state_info(mColorLogicOpEnabled,
                                              mColorLogicOp,
                                              mColorBlendAttachments,
                                              mBlendConstants);
}

auto GraphicsPipelineBuilder::get_dynamic_state_info() const
    -> VkPipelineDynamicStateCreateInfo
{
  return make_pipeline_dynamic_state_info(mDynamicStates);
}

}  // namespace grace
