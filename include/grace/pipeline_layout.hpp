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

#include <vector>  // vector

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

/**
 * Creates a push constant range specification.
 *
 * \param stages the shader stages that will access the push constant range.
 * \param offset the offset to the start of the range in bytes.
 * \param size   the size of the push constant range in bytes.
 *
 * \return a push constant range specification.
 */
[[nodiscard]] auto make_push_constant_range(VkShaderStageFlags stages,
                                            uint32 offset,
                                            uint32 size) -> VkPushConstantRange;

/**
 * Creates a pipeline layout specification.
 *
 * \param descriptor_set_layouts the associated descriptor set layouts.
 * \param push_constant_ranges   the associated push constant ranges.
 * \param flags                  the pipeline layout flags.
 *
 * \return a pipeline layout specification.
 */
[[nodiscard]] auto make_pipeline_layout_info(
    const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges,
    VkPipelineLayoutCreateFlags flags = 0) -> VkPipelineLayoutCreateInfo;

class PipelineLayout final {
 public:
  /**
   * Creates a pipeline layout.
   *
   * \param      device      the associated logical device.
   * \param      layout_info the pipeline layout specification.
   * \param[out] result      the resulting error code.
   *
   * \return a potentially null pipeline layout.
   *
   * \see PipelineLayoutBuilder
   */
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkPipelineLayoutCreateInfo& layout_info,
                                 VkResult* result = nullptr) -> PipelineLayout;

  PipelineLayout() noexcept = default;

  PipelineLayout(VkDevice device, VkPipelineLayout layout) noexcept;

  PipelineLayout(PipelineLayout&& other) noexcept;
  PipelineLayout(const PipelineLayout& other) = delete;

  auto operator=(PipelineLayout&& other) noexcept -> PipelineLayout&;
  auto operator=(const PipelineLayout& other) -> PipelineLayout& = delete;

  ~PipelineLayout() noexcept;

  /// Destroys the underlying pipeline layout.
  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkPipelineLayout { return mLayout; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkPipelineLayout() noexcept { return mLayout; }

  /// Indicates whether the underlying handle is non-null.
  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mLayout != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkPipelineLayout mLayout {VK_NULL_HANDLE};
};

class PipelineLayoutBuilder final {
 public:
  using Self = PipelineLayoutBuilder;

  PipelineLayoutBuilder(VkDevice device);

  /**
   * Resets the internal state.
   *
   * \details This function is only necessary if you want to use the builder to create
   *          multiple pipeline layout instances.
   *
   * \return the pipeline layout builder itself.
   */
  auto reset() -> Self&;

  /**
   * Includes a descriptor set layout in the pipeline layout.
   *
   * \param set_layout a descriptor set layout.
   *
   * \return the pipeline layout builder itself.
   */
  auto descriptor_set_layout(VkDescriptorSetLayout set_layout) -> Self&;

  /**
   * Adds a push constant range to the pipeline layout.
   *
   * \param push_constant_range a push constant range specification.
   *
   * \return the pipeline layout builder itself.
   */
  auto push_constant(const VkPushConstantRange& push_constant_range) -> Self&;

  /**
   * Adds a push constant range to the pipeline layout.
   *
   * \param stages the shader stages that will access the push constant range.
   * \param offset the offset to the start of the range in bytes.
   * \param size   the size of the push constant range in bytes.
   *
   * \return the pipeline layout builder itself.
   */
  auto push_constant(VkShaderStageFlags stages, uint32 offset, uint32 size) -> Self&;

  /**
   * Attempts to create the specified pipeline layout.
   *
   * \param[out] result the resulting error code.
   *
   * \return a potentially null pipeline layout.
   */
  [[nodiscard]] auto build(VkResult* result = nullptr) const -> PipelineLayout;

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
  std::vector<VkPushConstantRange> mPushConstantRanges;
};

}  // namespace grace
