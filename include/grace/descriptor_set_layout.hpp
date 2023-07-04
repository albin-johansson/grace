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
 * Creates a descriptor binding specification.
 *
 * \details This function uses the following defaults:\n
 * <ul>
 *   <li>The immutable sampler property is set to null.</li>
 * </ul>
 *
 * \param binding the descriptor binding identifier.
 * \param type    the descriptor type.
 * \param stages  the shader stages that can access the descriptor.
 * \param count   the number of descriptors in the binding.
 *
 * \return a descriptor set layout binding specification.
 */
[[nodiscard]] auto make_descriptor_set_layout_binding(uint32 binding,
                                                      VkDescriptorType type,
                                                      VkShaderStageFlags stages,
                                                      uint32 count = 1)
    -> VkDescriptorSetLayoutBinding;

[[nodiscard]] auto make_descriptor_set_layout_binding_flags_info(
    const std::vector<VkDescriptorBindingFlags>& binding_flags)
    -> VkDescriptorSetLayoutBindingFlagsCreateInfo;

[[nodiscard]] auto make_descriptor_set_layout_info(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings,
    VkDescriptorSetLayoutCreateFlags flags = 0,
    const void* next = nullptr) -> VkDescriptorSetLayoutCreateInfo;

class DescriptorSetLayout final {
 public:
  /**
   * Creates a descriptor set layout.
   *
   * \param      device      the associated logical device.
   * \param      layout_info the descriptor set layout specification.
   * \param[out] result      the resulting error code.
   *
   * \return a potentially null descriptor set layout.
   *
   * \see DescriptorSetLayoutBuilder
   */
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkDescriptorSetLayoutCreateInfo& layout_info,
                                 VkResult* result = nullptr) -> DescriptorSetLayout;

  DescriptorSetLayout() noexcept = default;

  DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout) noexcept;

  DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
  DescriptorSetLayout(const DescriptorSetLayout& other) = delete;

  auto operator=(DescriptorSetLayout&& other) noexcept -> DescriptorSetLayout&;
  auto operator=(const DescriptorSetLayout& other) -> DescriptorSetLayout& = delete;

  ~DescriptorSetLayout() noexcept;

  /// Destroys the underlying descriptor set layout.
  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkDescriptorSetLayout { return mLayout; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkDescriptorSetLayout() noexcept { return mLayout; }

  /// Indicates whether the internal handle is non-null.
  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mLayout != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkDescriptorSetLayout mLayout {VK_NULL_HANDLE};
};

/// A builder type that makes it easier to create descriptor set layouts.
class DescriptorSetLayoutBuilder final {
 public:
  using Self = DescriptorSetLayoutBuilder;

  DescriptorSetLayoutBuilder(VkDevice device);

  /**
   * Resets the internal state.
   *
   * \details This function is only necessary if you want to use the builder to create
   *          multiple descriptor set layout instances.
   *
   * \return the descriptor set layout builder itself.
   */
  auto reset() -> Self&;

  /**
   * Turns the descriptors in the layout into push descriptors.
   *
   * \note This requires the `VK_KHR_push_descriptor` device extension to be enabled.
   *
   * \return the descriptor set layout builder itself.
   */
  auto use_push_descriptors() -> Self&;

  /**
   * Allows for descriptors in the layout, such as samplers, to be partially bound.
   *
   * \note This might require the `VK_EXT_descriptor_indexing` device extension to be
   *       enabled on older Vulkan implementations.
   *
   * \return the descriptor set layout builder itself.
   */
  auto allow_partially_bound_descriptors() -> Self&;

  /**
   * Adds a descriptor binding to the layout.
   *
   * \param binding the descriptor binding specification.
   *
   * \return the descriptor set layout builder itself.
   */
  auto descriptor(const VkDescriptorSetLayoutBinding& binding) -> Self&;

  /**
   * Adds a descriptor binding to the layout.
   *
   * \param binding the descriptor binding identifier.
   * \param type    the descriptor type.
   * \param stages  the shader stages that can access the descriptor.
   * \param count   the number of descriptors in the binding.
   *
   * \return the descriptor set layout builder itself.
   */
  auto descriptor(uint32 binding,
                  VkDescriptorType type,
                  VkShaderStageFlags stages,
                  uint32 count = 1) -> Self&;

  /**
   * Attempts to create the specified descriptor set layout.
   *
   * \param[out] result the resulting error code.
   *
   * \return a potentially null descriptor set layout.
   */
  [[nodiscard]] auto build(VkResult* result = nullptr) const -> DescriptorSetLayout;

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayoutBinding> mBindings;
  bool mUsePushDescriptors             : 1 {false};
  bool mAllowPartiallyBoundDescriptors : 1 {false};

  [[nodiscard]] auto _make_descriptor_binding_flags() const
      -> std::vector<VkDescriptorBindingFlags>;
};

}  // namespace grace
