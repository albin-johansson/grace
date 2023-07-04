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
