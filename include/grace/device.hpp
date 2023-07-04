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

#include <memory>  // unique_ptr
#include <vector>  // vector

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

struct DeviceQueueCreateInfos final {
  float priority {1.0f};
  std::vector<VkDeviceQueueCreateInfo> queues;
};

[[nodiscard]] auto make_device_queue_info(uint32 queue_family_index,
                                          const float* priority)
    -> VkDeviceQueueCreateInfo;

[[nodiscard]] auto make_device_queue_infos(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> DeviceQueueCreateInfos;

/**
 * Returns creation information for a logical device.
 *
 * \param queue_infos      creation information for device queues.
 * \param layers           names of the required layers.
 * \param extensions       names of the required device extensions.
 * \param enabled_features physical device features that will be enabled.
 * \param next             a structure extension pointer.
 *
 * \return logical device creation information.
 */
[[nodiscard]] auto make_device_info(
    const std::vector<VkDeviceQueueCreateInfo>& queue_infos,
    const std::vector<const char*>& layers,
    const std::vector<const char*>& extensions,
    const VkPhysicalDeviceFeatures* enabled_features = nullptr,
    const void* next = nullptr) -> VkDeviceCreateInfo;

class Device final {
 public:
  /**
   * Attempts to create a Vulkan logical device.
   *
   * \param      gpu         the associated physical device.
   * \param      device_info the device specification.
   * \param[out] result      the resulting error code.
   *
   * \return a potentially null logical device.
   */
  [[nodiscard]] static auto make(VkPhysicalDevice gpu,
                                 const VkDeviceCreateInfo& device_info,
                                 VkResult* result = nullptr) -> Device;

  /**
   * Attempts to create a Vulkan logical device.
   *
   * \param      gpu              the associated physical device.
   * \param      surface          the associated surface.
   * \param      layers           the names of required layers.
   * \param      extensions       the names of the required extensions.
   * \param      enabled_features the enabled GPU features.
   * \param[out] result           the resulting error code.
   *
   * \return a potentially null logical device.
   */
  [[nodiscard]] static auto make(
      VkPhysicalDevice gpu,
      VkSurfaceKHR surface,
      const std::vector<const char*>& layers,
      const std::vector<const char*>& extensions,
      const VkPhysicalDeviceFeatures* enabled_features = nullptr,
      VkResult* result = nullptr) -> Device;

  /**
   * Returns a queue associated with the device.
   *
   * \param queue_family_index the family index of the desired queue.
   * \param queue_index        the index of the desired queue.
   *
   * \return a potentially null queue handle.
   */
  [[nodiscard]] auto get_queue(uint32 queue_family_index, uint32 queue_index = 0)
      -> VkQueue;

  [[nodiscard]] auto get() noexcept -> VkDevice { return mDevice.get(); }

  [[nodiscard]] operator VkDevice() noexcept { return mDevice.get(); }

  /// Indicates whether the device contains a non-null handle.
  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mDevice != VK_NULL_HANDLE;
  }

 private:
  struct DeviceDeleter final {
    void operator()(VkDevice device) noexcept;
  };

  std::unique_ptr<VkDevice_T, DeviceDeleter> mDevice;
};

}  // namespace grace