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
