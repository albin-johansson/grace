#pragma once

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_fence_info(VkFenceCreateFlags flags = 0) -> VkFenceCreateInfo;

class Fence final {
 public:
  Fence() noexcept = default;

  Fence(Fence&& other) noexcept;
  Fence(const Fence& other) = delete;

  Fence& operator=(Fence&& other) noexcept;
  Fence& operator=(const Fence& other) = delete;

  ~Fence() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkFenceCreateInfo& fence_info,
                                 VkResult* result = nullptr) -> Fence;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkFenceCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> Fence;

  void destroy() noexcept;

  auto wait(uint64 timeout = ~0) -> VkResult;

  auto reset() -> VkResult;

  [[nodiscard]] auto get() noexcept -> VkFence { return mFence; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mFence != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkFence mFence {VK_NULL_HANDLE};
};

}  // namespace grace
