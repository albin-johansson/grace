#pragma once

#include <memory>  // unique_ptr
#include <vector>  // vector

#include <vulkan/vulkan.h>

#include "grace/common.hpp"

namespace grace {

struct DeviceDeleter final {
  void operator()(VkDevice device) noexcept;
};

using UniqueDevice = std::unique_ptr<VkDevice_T, DeviceDeleter>;

[[nodiscard]] auto make_device_queue_info(uint32 queue_family_index,
                                          const float* priority)
    -> VkDeviceQueueCreateInfo;

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

struct DeviceResult final {
  UniqueDevice ptr;
  VkResult status;
};

/**
 * Attempts to create a Vulkan logical device.
 *
 * \param gpu              the associated physical device.
 * \param surface          the associated surface.
 * \param layers           names of the required layers.
 * \param extensions       names of the required device extensions.
 * \param enabled_features physical device features that will be enabled.
 * \param next             pointer to a structure extension for `VkDeviceCreateInfo`.
 *
 * \return the created logical device.
 */
[[nodiscard]] auto make_device(VkPhysicalDevice gpu,
                               VkSurfaceKHR surface,
                               const std::vector<const char*>& layers,
                               const std::vector<const char*>& extensions,
                               const VkPhysicalDeviceFeatures* enabled_features = nullptr,
                               const void* next = nullptr) -> DeviceResult;

}  // namespace grace