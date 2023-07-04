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

#include "grace/sampler.hpp"

namespace grace {

auto make_sampler_info(VkPhysicalDevice gpu,
                       const VkFilter min_filter,
                       const VkFilter mag_filter,
                       const VkSamplerAddressMode address_mode,
                       const float min_lod,
                       const float max_lod) -> VkSamplerCreateInfo
{
  VkPhysicalDeviceFeatures gpu_features = {};
  vkGetPhysicalDeviceFeatures(gpu, &gpu_features);

  VkPhysicalDeviceProperties gpu_properties = {};
  vkGetPhysicalDeviceProperties(gpu, &gpu_properties);

  return {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .magFilter = mag_filter,
      .minFilter = min_filter,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = address_mode,
      .addressModeV = address_mode,
      .addressModeW = address_mode,
      .mipLodBias = 0.0f,
      .anisotropyEnable = gpu_features.samplerAnisotropy,
      .maxAnisotropy = gpu_properties.limits.maxSamplerAnisotropy,
      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_NEVER,
      .minLod = min_lod,
      .maxLod = max_lod,
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
  };
}

Sampler::Sampler(VkDevice device, VkSampler sampler) noexcept
    : mDevice {device},
      mSampler {sampler}
{
}

Sampler::Sampler(Sampler&& other) noexcept
    : mDevice {other.mDevice},
      mSampler {other.mSampler}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mSampler = VK_NULL_HANDLE;
}

auto Sampler::operator=(Sampler&& other) noexcept -> Sampler&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mSampler = other.mSampler;

    other.mDevice = VK_NULL_HANDLE;
    other.mSampler = VK_NULL_HANDLE;
  }

  return *this;
}

Sampler::~Sampler() noexcept
{
  destroy();
}

void Sampler::destroy() noexcept
{
  if (mSampler != VK_NULL_HANDLE) {
    vkDestroySampler(mDevice, mSampler, nullptr);
    mSampler = VK_NULL_HANDLE;
  }
}

auto Sampler::make(VkDevice device,
                   const VkSamplerCreateInfo& sampler_info,
                   VkResult* result) -> Sampler
{
  VkSampler sampler_handle = VK_NULL_HANDLE;
  const auto status = vkCreateSampler(device, &sampler_info, nullptr, &sampler_handle);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return Sampler {device, sampler_handle};
  }

  return {};
}

auto Sampler::make(VkDevice device,
                   VkPhysicalDevice gpu,
                   const VkFilter filter_mode,
                   const VkSamplerAddressMode address_mode,
                   VkResult* result) -> Sampler
{
  const auto sampler_info =
      make_sampler_info(gpu, filter_mode, filter_mode, address_mode, 0.0f, 64.0f);
  return Sampler::make(device, sampler_info, result);
}

}  // namespace grace
