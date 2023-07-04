#include "grace/semaphore.hpp"

namespace grace {

auto make_semaphore_info(const VkSemaphoreCreateFlags flags) -> VkSemaphoreCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
  };
}

Semaphore::Semaphore(VkDevice device, VkSemaphore semaphore) noexcept
    : mDevice {device},
      mSemaphore {semaphore}
{
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : mDevice {other.mDevice},
      mSemaphore {other.mSemaphore}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mSemaphore = VK_NULL_HANDLE;
}

auto Semaphore::operator=(Semaphore&& other) noexcept -> Semaphore&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mSemaphore = other.mSemaphore;

    other.mDevice = VK_NULL_HANDLE;
    other.mSemaphore = VK_NULL_HANDLE;
  }

  return *this;
}

Semaphore::~Semaphore() noexcept
{
  destroy();
}

void Semaphore::destroy() noexcept
{
  if (mSemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(mDevice, mSemaphore, nullptr);
    mSemaphore = VK_NULL_HANDLE;
  }
}

auto Semaphore::make(VkDevice device,
                     const VkSemaphoreCreateInfo& semaphore_info,
                     VkResult* result) -> Semaphore
{
  VkSemaphore semaphore_handle = VK_NULL_HANDLE;
  const auto status =
      vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore_handle);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return Semaphore {device, semaphore_handle};
  }

  return {};
}

auto Semaphore::make(VkDevice device,
                     const VkSemaphoreCreateFlags flags,
                     VkResult* result) -> Semaphore
{
  const auto semaphore_info = make_semaphore_info(flags);
  return Semaphore::make(device, semaphore_info, result);
}

}  // namespace grace
