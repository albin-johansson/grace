#pragma once

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_semaphore_info(VkSemaphoreCreateFlags flags = 0)
    -> VkSemaphoreCreateInfo;

class Semaphore final {
 public:
  Semaphore() noexcept = default;

  Semaphore(VkDevice device, VkSemaphore semaphore) noexcept;

  Semaphore(Semaphore&& other) noexcept;
  Semaphore(const Semaphore& other) = delete;

  auto operator=(Semaphore&& other) noexcept -> Semaphore&;
  auto operator=(const Semaphore& other) -> Semaphore& = delete;

  ~Semaphore() noexcept;

  [[nodiscard]] static auto make(VkDevice device,
                                 const VkSemaphoreCreateInfo& semaphore_info,
                                 VkResult* result = nullptr) -> Semaphore;

  [[nodiscard]] static auto make(VkDevice device,
                                 VkSemaphoreCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> Semaphore;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkSemaphore { return mSemaphore; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mSemaphore != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkSemaphore mSemaphore {VK_NULL_HANDLE};
};

}  // namespace grace
