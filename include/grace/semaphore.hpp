#pragma once

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_semaphore_info(VkSemaphoreCreateFlags flags = 0)
    -> VkSemaphoreCreateInfo;

class Semaphore final {
 public:
  Semaphore() noexcept = default;

  Semaphore(Semaphore&& other) noexcept;
  Semaphore(const Semaphore& other) = delete;

  Semaphore& operator=(Semaphore&& other) noexcept;
  Semaphore& operator=(const Semaphore& other) = delete;

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
