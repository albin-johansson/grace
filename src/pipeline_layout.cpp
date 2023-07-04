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

#include "grace/pipeline_layout.hpp"

namespace grace {

auto make_push_constant_range(const VkShaderStageFlags stages,
                              const uint32 offset,
                              const uint32 size) -> VkPushConstantRange
{
  return {
      .stageFlags = stages,
      .offset = offset,
      .size = size,
  };
}

auto make_pipeline_layout_info(
    const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges,
    const VkPipelineLayoutCreateFlags flags) -> VkPipelineLayoutCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
      .setLayoutCount = u32_size(descriptor_set_layouts),
      .pSetLayouts = data_or_null(descriptor_set_layouts),
      .pushConstantRangeCount = u32_size(push_constant_ranges),
      .pPushConstantRanges = data_or_null(push_constant_ranges),
  };
}

PipelineLayout::PipelineLayout(VkDevice device, VkPipelineLayout layout) noexcept
    : mDevice {device},
      mLayout {layout}
{
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : mDevice {other.mDevice},
      mLayout {other.mLayout}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mLayout = VK_NULL_HANDLE;
}

auto PipelineLayout::operator=(PipelineLayout&& other) noexcept -> PipelineLayout&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mLayout = other.mLayout;

    other.mDevice = VK_NULL_HANDLE;
    other.mLayout = VK_NULL_HANDLE;
  }

  return *this;
}

PipelineLayout::~PipelineLayout() noexcept
{
  destroy();
}

void PipelineLayout::destroy() noexcept
{
  if (mLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(mDevice, mLayout, nullptr);
    mLayout = VK_NULL_HANDLE;
  }
}

auto PipelineLayout::make(VkDevice device,
                          const VkPipelineLayoutCreateInfo& layout_info,
                          VkResult* result) -> PipelineLayout
{
  VkPipelineLayout layout = VK_NULL_HANDLE;
  const auto status = vkCreatePipelineLayout(device, &layout_info, nullptr, &layout);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return PipelineLayout {device, layout};
  }

  return {};
}

PipelineLayoutBuilder::PipelineLayoutBuilder(VkDevice device)
    : mDevice {device}
{
}

auto PipelineLayoutBuilder::reset() -> Self&
{
  mDescriptorSetLayouts.clear();
  mPushConstantRanges.clear();

  return *this;
}

auto PipelineLayoutBuilder::descriptor_set_layout(VkDescriptorSetLayout set_layout)
    -> Self&
{
  mDescriptorSetLayouts.push_back(set_layout);
  return *this;
}

auto PipelineLayoutBuilder::push_constant(const VkPushConstantRange& push_constant_range)
    -> Self&
{
  mPushConstantRanges.push_back(push_constant_range);
  return *this;
}

auto PipelineLayoutBuilder::push_constant(const VkShaderStageFlags stages,
                                          const uint32 offset,
                                          const uint32 size) -> Self&
{
  return push_constant(make_push_constant_range(stages, offset, size));
}

auto PipelineLayoutBuilder::build(VkResult* result) const -> PipelineLayout
{
  const auto layout_info =
      make_pipeline_layout_info(mDescriptorSetLayouts, mPushConstantRanges);
  return PipelineLayout::make(mDevice, layout_info, result);
}

}  // namespace grace
