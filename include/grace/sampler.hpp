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

[[nodiscard]] auto make_sampler_info(VkPhysicalDevice gpu,
                                     VkFilter min_filter,
                                     VkFilter mag_filter,
                                     VkSamplerAddressMode address_mode,
                                     float min_lod,
                                     float max_lod) -> VkSamplerCreateInfo;

class Sampler final {
 public:
  Sampler() = default;

  Sampler(VkDevice device, VkSampler sampler) noexcept;

  Sampler(Sampler&& other) noexcept;
  Sampler(const Sampler& other) = delete;

  auto operator=(Sampler&& other) noexcept -> Sampler&;
  auto operator=(const Sampler& other) -> Sampler& = delete;

  ~Sampler() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkSamplerCreateInfo& sampler_info,
                                 VkResult* result = nullptr) -> Sampler;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkPhysicalDevice gpu,
                                 VkFilter filter_mode,
                                 VkSamplerAddressMode address_mode,
                                 VkResult* result = nullptr) -> Sampler;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkSampler { return mSampler; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkSampler() noexcept { return mSampler; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mSampler != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkSampler mSampler {VK_NULL_HANDLE};
};

}  // namespace grace
