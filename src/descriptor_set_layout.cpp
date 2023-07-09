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

#include "grace/descriptor_set_layout.hpp"

namespace grace {

auto make_descriptor_set_layout_binding(const uint32 binding,
                                        const VkDescriptorType type,
                                        const VkShaderStageFlags stages,
                                        const uint32 count)
    -> VkDescriptorSetLayoutBinding
{
  return {
      .binding = binding,
      .descriptorType = type,
      .descriptorCount = count,
      .stageFlags = stages,
      .pImmutableSamplers = nullptr,
  };
}

auto make_descriptor_set_layout_binding_flags_info(
    const std::vector<VkDescriptorBindingFlags>& binding_flags)
    -> VkDescriptorSetLayoutBindingFlagsCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
      .pNext = nullptr,
      .bindingCount = u32_size(binding_flags),
      .pBindingFlags = data_or_null(binding_flags),
  };
}

auto make_descriptor_set_layout_info(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    const VkDescriptorSetLayoutCreateFlags flags,
    const void* next) -> VkDescriptorSetLayoutCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = next,
      .flags = flags,
      .bindingCount = u32_size(bindings),
      .pBindings = data_or_null(bindings),
  };
}

DescriptorSetLayout::DescriptorSetLayout(VkDevice device,
                                         VkDescriptorSetLayout layout) noexcept
    : mDevice {device},
      mLayout {layout}
{
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : mDevice {other.mDevice},
      mLayout {other.mLayout}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mLayout = VK_NULL_HANDLE;
}

auto DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept
    -> DescriptorSetLayout&
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

DescriptorSetLayout::~DescriptorSetLayout() noexcept
{
  destroy();
}

void DescriptorSetLayout::destroy() noexcept
{
  if (mLayout != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(mDevice, mLayout, nullptr);
    mLayout = VK_NULL_HANDLE;
  }
}

auto DescriptorSetLayout::make(VkDevice device,
                               const VkDescriptorSetLayoutCreateInfo& layout_info,
                               VkResult* result) -> DescriptorSetLayout
{
  VkDescriptorSetLayout layout = VK_NULL_HANDLE;
  const auto status = vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &layout);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return DescriptorSetLayout {device, layout};
  }

  return {};
}

DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(VkDevice device)
    : mDevice {device}
{
}

auto DescriptorSetLayoutBuilder::reset() -> Self&
{
  mBindings.clear();
  mUsePushDescriptors = false;

  return *this;
}

auto DescriptorSetLayoutBuilder::use_push_descriptors() -> Self&
{
  mUsePushDescriptors = true;
  return *this;
}

auto DescriptorSetLayoutBuilder::allow_partially_bound_descriptors() -> Self&
{
  mAllowPartiallyBoundDescriptors = true;
  return *this;
}

auto DescriptorSetLayoutBuilder::descriptor(const VkDescriptorSetLayoutBinding& binding)
    -> Self&
{
  mBindings.push_back(binding);
  return *this;
}

auto DescriptorSetLayoutBuilder::descriptor(const uint32 binding,
                                            const VkDescriptorType type,
                                            const VkShaderStageFlags stages,
                                            const uint32 count) -> Self&
{
  return descriptor(make_descriptor_set_layout_binding(binding, type, stages, count));
}

auto DescriptorSetLayoutBuilder::build(VkResult* result) const -> DescriptorSetLayout
{
  const auto descriptor_binding_flags = _make_descriptor_binding_flags();
  const auto binding_flags_info =
      make_descriptor_set_layout_binding_flags_info(descriptor_binding_flags);

  VkDescriptorSetLayoutCreateFlags flags = 0;
  if (mUsePushDescriptors) {
    flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
  }

  const auto layout_info =
      make_descriptor_set_layout_info(mBindings, flags, &binding_flags_info);

  return DescriptorSetLayout::make(mDevice, layout_info, result);
}

auto DescriptorSetLayoutBuilder::_make_descriptor_binding_flags() const
    -> std::vector<VkDescriptorBindingFlags>
{
  std::vector<VkDescriptorBindingFlags> descriptor_binding_flags;
  descriptor_binding_flags.reserve(mBindings.size());

  if (mAllowPartiallyBoundDescriptors) {
    descriptor_binding_flags.assign(mBindings.size(),
                                    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
  }
  else {
    descriptor_binding_flags.assign(mBindings.size(), 0);
  }

  return descriptor_binding_flags;
}

}  // namespace grace
