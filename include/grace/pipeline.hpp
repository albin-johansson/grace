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

#include <array>     // array
#include <optional>  // optional
#include <string>    // string
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_viewport(float x,
                                 float y,
                                 float width,
                                 float height,
                                 float min_depth = 0.0,
                                 float max_depth = 1.0f) -> VkViewport;

[[nodiscard]] auto make_rect_2d(int32 x, int32 y, uint32 width, uint32 height)
    -> VkRect2D;

[[nodiscard]] auto make_pipeline_shader_stage_info(
    VkShaderStageFlagBits stage,
    VkShaderModule shader_module,
    const VkSpecializationInfo* specialization = nullptr,
    const char* entry_point = "main") -> VkPipelineShaderStageCreateInfo;

[[nodiscard]] auto make_pipeline_vertex_input_state_info(
    const std::vector<VkVertexInputBindingDescription>& bindings,
    const std::vector<VkVertexInputAttributeDescription>& attributes)
    -> VkPipelineVertexInputStateCreateInfo;

[[nodiscard]] auto make_pipeline_input_assembly_state_info(VkPrimitiveTopology topology)
    -> VkPipelineInputAssemblyStateCreateInfo;

[[nodiscard]] auto make_pipeline_tessellation_state_info(uint32 patch_control_points)
    -> VkPipelineTessellationStateCreateInfo;

[[nodiscard]] auto make_pipeline_viewport_state_info(
    const std::vector<VkViewport>& viewports,
    const std::vector<VkRect2D>& scissors) -> VkPipelineViewportStateCreateInfo;

[[nodiscard]] auto make_pipeline_color_blend_state_info(
    bool op_enabled,
    VkLogicOp op,
    const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
    const std::array<float, 4>& blend_constants) -> VkPipelineColorBlendStateCreateInfo;

[[nodiscard]] auto make_pipeline_dynamic_state_info(
    const std::vector<VkDynamicState>& states) -> VkPipelineDynamicStateCreateInfo;

class Pipeline {
 public:
  Pipeline() noexcept = default;

  Pipeline(VkDevice device, VkPipeline pipeline) noexcept;

  Pipeline(Pipeline&& other) noexcept;
  Pipeline(const Pipeline& other) = delete;

  auto operator=(Pipeline&& other) noexcept -> Pipeline&;
  auto operator=(const Pipeline& other) -> Pipeline& = delete;

  ~Pipeline() noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkPipeline { return mPipeline; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkPipeline() noexcept { return mPipeline; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mPipeline != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkPipeline mPipeline {VK_NULL_HANDLE};
};

class GraphicsPipeline final : public Pipeline {
 public:
  using Pipeline::Pipeline;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkGraphicsPipelineCreateInfo& pipeline_info,
                                 VkPipelineCache cache = VK_NULL_HANDLE,
                                 VkResult* result = nullptr) -> GraphicsPipeline;
};

class GraphicsPipelineBuilder final {
 public:
  using Self = GraphicsPipelineBuilder;

  inline static constexpr auto kDefaultTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inline static constexpr auto kDefaultPolygonMode = VK_POLYGON_MODE_FILL;
  inline static constexpr auto kDefaultCullMode = VK_CULL_MODE_NONE;
  inline static constexpr auto kDefaultFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  inline static constexpr auto kDefaultDepthCompareOp = VK_COMPARE_OP_LESS;
  inline static constexpr auto kDefaultColorLogicOp = VK_LOGIC_OP_NO_OP;
  inline static constexpr float kDefaultLineWidth = 1.0f;
  inline static constexpr float kDefaultMinDepth = 0.0f;
  inline static constexpr float kDefaultMaxDepth = 1.0f;

  explicit GraphicsPipelineBuilder(VkDevice device);

  /**
   * Resets the internal state.
   *
   * \return the pipeline builder itself.
   */
  auto reset() -> Self&;

  /**
   * Specifies the pipeline layout.
   *
   * \note This function must be called in order to be able to build the pipeline.
   *
   * \param layout the associated pipeline layout.
   *
   * \return the pipeline builder itself.
   */
  auto with_layout(VkPipelineLayout layout) -> Self&;

  /**
   * Specifies the associated pipeline cache.
   *
   * \param cache a pipeline cache.
   *
   * \return the pipeline builder itself.
   */
  auto with_cache(VkPipelineCache cache) -> Self&;

  /**
   * Specifies the associated render pass.
   *
   * \note This function must be called in order to be able to build the pipeline.
   *
   * \param render_pass the associated render pass handle.
   * \param subpass     the index of the subpass in which the pipeline will be used.
   *
   * \return the pipeline builder itself.
   */
  auto with_render_pass(VkRenderPass render_pass, uint32 subpass) -> Self&;

  /**
   * Specifies the vertex shader that will be used.
   *
   * \note This function must be called in order to be able to build the pipeline.
   *
   * \param shader_path the file path to the compiled vertex shader.
   * \param entry_name  the name of the entry point function.
   *
   * \return the pipeline builder itself.
   */
  auto vertex_shader(const char* shader_path, const char* entry_name = "main") -> Self&;

  /**
   * Specifies the fragment shader that will be used.
   *
   * \note This function must be called in order to be able to build the pipeline.
   *
   * \param shader_path the file path to the compiled fragment shader.
   * \param entry_name  the name of the entry point function.
   *
   * \return the pipeline builder itself.
   */
  auto fragment_shader(const char* shader_path, const char* entry_name = "main") -> Self&;

  auto vertex_input_binding(uint32 binding,
                            uint32 stride,
                            VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX)
      -> Self&;

  auto vertex_attribute(uint32 binding, uint32 location, VkFormat format, uint32 offset)
      -> Self&;

  /**
   * Specifies the format of vertex data.
   *
   * \details This property is by default set to `VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST`.
   *
   * \param topology the vertex data format.
   *
   * \return the pipeline builder itself.
   */
  auto primitive_topology(VkPrimitiveTopology topology) -> Self&;

  /**
   * Sets the tessellation patch control points.
   *
   * \param patch_control_points the number of control points per patch.
   *
   * \return the pipeline builder itself.
   */
  auto tessellation(uint32 patch_control_points) -> Self&;

  /**
   * Adds a viewport to the pipeline.
   *
   * \details This property is ignored if you specify that the viewport state is dynamic.
   *
   * \note You can call this function several times to specify multiple viewports.
   *
   * \param x         the viewport x-coordinate.
   * \param y         the viewport y-coordinate.
   * \param width     the width of the viewport.
   * \param height    the height of the viewport.
   * \param min_depth the minimum depth value in the viewport.
   * \param max_depth the maximum depth value in the viewport.
   *
   * \return the pipeline builder itself.
   */
  auto viewport(float x,
                float y,
                float width,
                float height,
                float min_depth = kDefaultMinDepth,
                float max_depth = kDefaultMaxDepth) -> Self&;

  /**
   * Adds a scissor to the pipeline.
   *
   * \details This property is ignored if you specify that the scissor state is dynamic.
   *
   * \note You can call this function several times to specify multiple scissors.
   *
   * \param x         the x-axis offset of the scissor.
   * \param y         the y-axis offset of the scissor.
   * \param width     the width of the scissor.
   * \param height    the height of the scissor.
   *
   * \return the pipeline builder itself.
   */
  auto scissor(int32 x, int32 y, uint32 width, uint32 height) -> Self&;

  /**
   * Specifies rasterization options.
   *
   * \details The following values are used by the builder by default.
   * <ul>
   *   <li>The polygon mode is `VK_POLYGON_MODE_FILL`.</li>
   *   <li>The cull mode is `VK_CULL_MODE_NONE`.</li>
   *   <li>The front face is `VK_FRONT_FACE_COUNTER_CLOCKWISE`.</li>
   * </ul>
   *
   * \param polygon_mode the triangle rendering mode.
   * \param cull_mode    the primitive culling mode.
   * \param front_face   the winding order used to determine the front faces of triangles.
   *
   * \return the pipeline builder itself.
   */
  auto rasterization(VkPolygonMode polygon_mode,
                     VkCullModeFlags cull_mode = kDefaultCullMode,
                     VkFrontFace front_face = kDefaultFrontFace) -> Self&;

  auto color_logic_op(bool enabled, VkLogicOp op = kDefaultColorLogicOp) -> Self&;

  auto blend_constants(float red, float green, float blue, float alpha) -> Self&;

  auto color_blend_attachment(const VkPipelineColorBlendAttachmentState& attachment)
      -> Self&;

  auto color_blend_attachment(
      bool enabled,
      VkBlendOp op = VK_BLEND_OP_ADD,
      VkBlendFactor src_color_factor = VK_BLEND_FACTOR_SRC_ALPHA,
      VkBlendFactor dst_color_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      VkBlendFactor src_alpha_factor = VK_BLEND_FACTOR_ONE,
      VkBlendFactor dst_alpha_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA) -> Self&;

  /**
   * Specifies the width of rasterized line segments.
   *
   * \details The property is by default set to 1.
   *
   * \param width the line width.
   *
   * \return the pipeline builder itself.
   */
  auto line_width(float width) -> Self&;

  /**
   * Controls fragment depth bias calculations.
   *
   * \details Depth bias calculations are disabled by default.
   *
   * \param enabled         whether fragment depth bias is enabled.
   * \param constant_factor the depth value added to each fragment.
   * \param slope_factor    the value added to a fragment’s "slope" in bias calculations.
   * \param clamp_value     the maximum/minimum depth bias for a fragment.
   *
   * \return the pipeline builder itself.
   */
  auto depth_bias(bool enabled,
                  float constant_factor = 0,
                  float slope_factor = 0,
                  float clamp_value = 0) -> Self&;

  /**
   * Controls fragment depth testing.
   *
   * \details Depth testing is disabled by default.
   *
   * \param enabled    whether fragment depth testing is enabled.
   * \param compare_op the depth comparison operator.
   *
   * \return the pipeline builder itself.
   */
  auto depth_test(bool enabled, VkCompareOp compare_op = kDefaultDepthCompareOp) -> Self&;

  /**
   * Controls depth bounds testing.
   *
   * \details Depth bounds testing is disabled by default.
   *
   * \param enabled   whether depth bound testing is enabled.
   * \param min_depth the minimum depth value.
   * \param max_depth the maximum depth value.
   *
   * \return the pipeline builder itself.
   */
  auto depth_bounds_test(bool enabled,
                         float min_depth = kDefaultMinDepth,
                         float max_depth = kDefaultMaxDepth) -> Self&;

  /**
   * Controls depth fragment writes.
   *
   * \details Fragment depth writes are enabled by default.
   *
   * \param enabled whether fragment depth writes are enabled.
   *
   * \return the pipeline builder itself.
   */
  auto depth_write(bool enabled) -> Self&;

  /**
   * Controls fragment depth value clamping.
   *
   * \details Depth value clamping is disabled by default.
   *
   * \param enabled whether fragment depth values are clamped.
   *
   * \return the pipeline builder itself.
   */
  auto depth_clamp(bool enabled) -> Self&;

  /**
   * Controls stencil testing.
   *
   * \details Stencil testing is disabled by default.
   *
   * \param enabled whether stencil testing is enabled.
   * \param front   the front stencil operation specification.
   * \param back    the back stencil operation specification.
   *
   * \return the pipeline builder itself.
   */
  auto stencil_test(bool enabled,
                    const VkStencilOpState& front = {},
                    const VkStencilOpState& back = {}) -> Self&;

  /**
   * Specifies that a particular piece of pipeline state should be fetched dynamically.
   *
   * \note You can call this function several times to specify multiple dynamic states.
   *
   * \param state the pipeline state to mark as dynamic.
   *
   * \return the pipeline builder itself.
   */
  auto dynamic_state(VkDynamicState state) -> Self&;

  /**
   * Attempts to create the specified pipeline.
   *
   * \param[out] result the resulting error code.
   *
   * \return a potentially null graphics pipeline.
   */
  [[nodiscard]] auto build(VkResult* result = nullptr) const -> GraphicsPipeline;

  [[nodiscard]] auto get_vertex_input_state_info() const
      -> VkPipelineVertexInputStateCreateInfo;

  [[nodiscard]] auto get_input_assembly_state_info() const
      -> VkPipelineInputAssemblyStateCreateInfo;

  [[nodiscard]] auto get_tessellation_state_info() const
      -> VkPipelineTessellationStateCreateInfo;

  [[nodiscard]] auto get_viewport_state_info() const -> VkPipelineViewportStateCreateInfo;

  [[nodiscard]] auto get_rasterization_state_info() const
      -> VkPipelineRasterizationStateCreateInfo;

  [[nodiscard]] auto get_multisample_state_info() const
      -> VkPipelineMultisampleStateCreateInfo;

  [[nodiscard]] auto get_depth_stencil_state_info() const
      -> VkPipelineDepthStencilStateCreateInfo;

  [[nodiscard]] auto get_color_blend_state_info() const
      -> VkPipelineColorBlendStateCreateInfo;

  [[nodiscard]] auto get_dynamic_state_info() const -> VkPipelineDynamicStateCreateInfo;

 private:
  struct ShaderInfo final {
    std::string path;
    std::string entry_name;
  };

  VkDevice mDevice {VK_NULL_HANDLE};
  VkPipelineLayout mLayout {VK_NULL_HANDLE};
  VkPipelineCache mCache {VK_NULL_HANDLE};
  VkRenderPass mRenderPass {VK_NULL_HANDLE};

  ShaderInfo mVertexShader;
  ShaderInfo mFragmentShader;

  std::vector<VkVertexInputBindingDescription> mVertexInputBindings;
  std::vector<VkVertexInputAttributeDescription> mVertexAttributes;
  std::vector<VkViewport> mViewports;
  std::vector<VkRect2D> mScissors;
  std::vector<VkDynamicState> mDynamicStates;
  std::vector<VkPipelineColorBlendAttachmentState> mColorBlendAttachments;

  std::optional<uint32> mTessellationPatchControlPoints;

  uint32 mSubpass {0};
  VkPrimitiveTopology mPrimitiveTopology {kDefaultTopology};
  VkPolygonMode mPolygonMode {kDefaultPolygonMode};
  VkCullModeFlags mCullMode {kDefaultCullMode};
  VkFrontFace mFrontFace {kDefaultFrontFace};
  VkCompareOp mDepthCompareOp {kDefaultDepthCompareOp};
  VkLogicOp mColorLogicOp {kDefaultColorLogicOp};
  VkStencilOpState mFrontStencilOpState {};
  VkStencilOpState mBackStencilOpState {};

  float mLineWidth {kDefaultLineWidth};
  float mDepthBiasConstantFactor {};
  float mDepthBiasSlopeFactor {};
  float mDepthBiasClampValue {};
  float mMinDepth {kDefaultMinDepth};
  float mMaxDepth {kDefaultMaxDepth};
  std::array<float, 4> mBlendConstants {};

  bool mDepthBiasEnabled       : 1 {false};
  bool mDepthTestEnabled       : 1 {false};
  bool mDepthWriteEnabled      : 1 {false};
  bool mDepthBoundsTestEnabled : 1 {false};
  bool mDepthClampEnabled      : 1 {false};
  bool mStencilTestEnabled     : 1 {false};
  bool mColorLogicOpEnabled    : 1 {false};

  [[nodiscard]] auto _is_complete() const -> bool;
};

}  // namespace grace
