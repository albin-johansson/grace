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

#include "grace/fence.hpp"

namespace grace {

auto make_fence_info(const VkFenceCreateFlags flags) -> VkFenceCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
  };
}

Fence::Fence(Fence&& other) noexcept
    : mDevice {other.mDevice},
      mFence {other.mFence}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mFence = VK_NULL_HANDLE;
}

Fence& Fence::operator=(Fence&& other) noexcept
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mFence = other.mFence;

    other.mDevice = VK_NULL_HANDLE;
    other.mFence = VK_NULL_HANDLE;
  }

  return *this;
}

Fence::~Fence() noexcept
{
  destroy();
}

void Fence::destroy() noexcept
{
  if (mFence != VK_NULL_HANDLE) {
    vkDestroyFence(mDevice, mFence, nullptr);
    mFence = VK_NULL_HANDLE;
  }
}

auto Fence::make(VkDevice device, const VkFenceCreateInfo& fence_info, VkResult* result)
    -> Fence
{
  Fence fence;
  fence.mDevice = device;
  const auto status = vkCreateFence(device, &fence_info, nullptr, &fence.mFence);

  if (result) {
    *result = status;
  }

  if (status != VK_SUCCESS) {
    return {};
  }

  return fence;
}

auto Fence::make(VkDevice device, const VkFenceCreateFlags flags, VkResult* result)
    -> Fence
{
  const auto fence_info = make_fence_info(flags);
  return Fence::make(device, fence_info, result);
}

auto Fence::wait(const uint64 timeout) -> VkResult
{
  return vkWaitForFences(mDevice, 1, &mFence, VK_TRUE, timeout);
}

auto Fence::reset() -> VkResult
{
  return vkResetFences(mDevice, 1, &mFence);
}

}  // namespace grace