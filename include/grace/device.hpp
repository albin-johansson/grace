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

struct DeviceSpec final {
  std::vector<const char*> layers;                      ///< Names of required layers.
  std::vector<const char*> extensions;                  ///< Names of required extensions.
  const VkPhysicalDeviceFeatures* enabled_features {};  ///< Enabled GPU features.
  const void* next {};  ///< Any `VkDeviceCreateInfo` extension.
};

/**
 * Attempts to create a Vulkan logical device.
 *
 * \param      gpu     the associated physical device.
 * \param      surface the associated surface.
 * \param      spec    the device specification.
 * \param[out] result  the resulting error code.
 *
 * \return a logical device on success; a null pointer on failure.
 */
[[nodiscard]] auto make_device(VkPhysicalDevice gpu,
                               VkSurfaceKHR surface,
                               const DeviceSpec& spec,
                               VkResult* result = nullptr) -> UniqueDevice;

}  // namespace grace