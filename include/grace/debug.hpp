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
