#pragma once

#include <functional>  // function
#include <optional>    // optional
#include <vector>      // vector

#include <vulkan/vulkan.h>

#include "grace/common.hpp"

namespace grace {

struct QueueFamilyIndices final {
  std::optional<uint32> graphics;  ///< The graphics family index.
  std::optional<uint32> present;   ///< The presentation family index.
};

struct SwapchainSupport final {
  VkSurfaceCapabilitiesKHR surface_capabilities {};
  std::vector<VkSurfaceFormatKHR> surface_formats;  ///< Supported surface formats.
  std::vector<VkPresentModeKHR> present_modes;      ///< Supported present modes.
};

/// Returns all available GPUs, regardless of their suitability.
[[nodiscard]] auto get_physical_devices(VkInstance instance)
    -> std::vector<VkPhysicalDevice>;

[[nodiscard]] auto get_extensions(VkPhysicalDevice gpu)
    -> std::vector<VkExtensionProperties>;

[[nodiscard]] auto get_queue_families(VkPhysicalDevice gpu)
    -> std::vector<VkQueueFamilyProperties>;

[[nodiscard]] auto get_surface_formats(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> std::vector<VkSurfaceFormatKHR>;

[[nodiscard]] auto get_present_modes(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> std::vector<VkPresentModeKHR>;

[[nodiscard]] auto get_queue_family_indices(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> QueueFamilyIndices;

[[nodiscard]] auto get_unique_queue_family_indices(VkPhysicalDevice gpu,
                                                   VkSurfaceKHR surface)
    -> std::vector<uint32>;

[[nodiscard]] auto get_swapchain_support(VkPhysicalDevice gpu, VkSurfaceKHR surface)
    -> SwapchainSupport;

using PhysicalDeviceFilter = std::function<bool(VkPhysicalDevice, VkSurfaceKHR)>;
using PhysicalDeviceRater = std::function<int(VkPhysicalDevice)>;

/**
 * Attempts to select one of the available physical devices.
 *
 * \details This function determines all potentially suitable GPUs using the
 *          `gpu_filter` predicate, and obtains the suitability score from each such
 *          GPU using the `gpu_rater` function. The preferred GPU is subsequently
 *          determined by selecting the GPU with the highest score.
 *
 * \param instance   the associated Vulkan instance.
 * \param surface    the associated Vulkan surface.
 * \param gpu_filter a predicate used to determine whether a GPU is suitable for use.
 * \param gpu_rater  a function used to obtain a score of how suitable a GPU is (higher
 *                   values are better).
 *
 * \return the selected physical device, or null if no suitable GPU was found.
 */
[[nodiscard]] auto pick_physical_device(VkInstance instance,
                                        VkSurfaceKHR surface,
                                        const PhysicalDeviceFilter& gpu_filter,
                                        const PhysicalDeviceRater& gpu_rater)
    -> VkPhysicalDevice;

}  // namespace grace
