#pragma once

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_debug_utils_messenger_info(
    VkDebugUtilsMessageSeverityFlagsEXT severities,
    VkDebugUtilsMessageTypeFlagsEXT types,
    PFN_vkDebugUtilsMessengerCallbackEXT callback,
    void* user_data = nullptr) -> VkDebugUtilsMessengerCreateInfoEXT;

VKAPI_ATTR VkBool32 VKAPI_CALL
debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                               VkDebugUtilsMessageTypeFlagsEXT msg_type,
                               const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                               void* user_data);

struct DebugMessengerFunctions final {
  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT {nullptr};
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT {nullptr};
};

[[nodiscard]] auto get_debug_messenger_functions(VkInstance instance)
    -> DebugMessengerFunctions;

class DebugMessenger final {
 public:
  inline static constexpr VkDebugUtilsMessageSeverityFlagsEXT kDefaultMsgSeverities =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  inline static constexpr VkDebugUtilsMessageTypeFlagsEXT kDefaultMsgTypes =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  [[nodiscard]] static auto make(VkInstance instance,
                                 const VkDebugUtilsMessengerCreateInfoEXT& messenger_info,
                                 VkResult* result = nullptr) -> DebugMessenger;

  [[nodiscard]] static auto make(
      VkInstance instance,
      VkDebugUtilsMessageSeverityFlagsEXT severities = kDefaultMsgSeverities,
      VkDebugUtilsMessageTypeFlagsEXT types = kDefaultMsgTypes,
      PFN_vkDebugUtilsMessengerCallbackEXT callback = &debug_utils_messenger_callback,
      VkResult* result = nullptr) -> DebugMessenger;

  DebugMessenger() noexcept = default;

  DebugMessenger(DebugMessengerFunctions functions,
                 VkInstance instance,
                 VkDebugUtilsMessengerEXT messenger) noexcept;

  DebugMessenger(DebugMessenger&& other) noexcept;
  DebugMessenger(const DebugMessenger& other) = delete;

  auto operator=(DebugMessenger&& other) noexcept -> DebugMessenger&;
  auto operator=(const DebugMessenger& other) -> DebugMessenger& = delete;

  ~DebugMessenger() noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkDebugUtilsMessengerEXT { return mMessenger; }

  [[nodiscard]] auto instance() noexcept -> VkInstance { return mInstance; }

  [[nodiscard]] operator VkDebugUtilsMessengerEXT() noexcept { return mMessenger; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mMessenger != VK_NULL_HANDLE;
  }

 private:
  DebugMessengerFunctions mFunctions;
  VkInstance mInstance {VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT mMessenger {VK_NULL_HANDLE};
};

}  // namespace grace
