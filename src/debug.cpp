#include "grace/debug.hpp"

#include <iostream>  // cout, cerr
#include <ostream>   // flush

namespace grace {

auto make_debug_utils_messenger_info(const VkDebugUtilsMessageSeverityFlagsEXT severities,
                                     const VkDebugUtilsMessageTypeFlagsEXT types,
                                     PFN_vkDebugUtilsMessengerCallbackEXT callback,
                                     void* user_data)
    -> VkDebugUtilsMessengerCreateInfoEXT
{
  return {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = severities,
      .messageType = types,
      .pfnUserCallback = callback,
      .pUserData = user_data,
  };
}

auto debug_utils_messenger_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    const VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void*) -> VkBool32
{
  const char* type_prefix = "[UNKNOWN]";
  if (msg_type == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
    type_prefix = "[GENERAL]";
  }
  else if (msg_type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
    type_prefix = "[VALIDATION]";
  }
  else if (msg_type == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
    type_prefix = "[PERFORMANCE]";
  }

  auto& stream = (msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ||
                  msg_type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
                     ? std::cerr
                     : std::cout;

  stream << type_prefix << ' ' << callback_data->pMessage << '\n';

  if (msg_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    stream << std::flush;
  }

  return VK_FALSE;
}

auto get_debug_messenger_functions(VkInstance instance) -> DebugMessengerFunctions
{
  DebugMessengerFunctions functions;

  functions.vkCreateDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

  functions.vkDestroyDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

  return functions;
}

DebugMessenger::DebugMessenger(DebugMessengerFunctions functions,
                               VkInstance instance,
                               VkDebugUtilsMessengerEXT messenger) noexcept
    : mFunctions {functions},
      mInstance {instance},
      mMessenger {messenger}
{
}

DebugMessenger::DebugMessenger(DebugMessenger&& other) noexcept
    : mFunctions {other.mFunctions},
      mInstance {other.mInstance},
      mMessenger {other.mMessenger}
{
  other.mInstance = VK_NULL_HANDLE;
  other.mMessenger = VK_NULL_HANDLE;
}

auto DebugMessenger::operator=(DebugMessenger&& other) noexcept -> DebugMessenger&
{
  if (this != &other) {
    destroy();

    mFunctions = other.mFunctions;
    mInstance = other.mInstance;
    mMessenger = other.mMessenger;

    other.mInstance = VK_NULL_HANDLE;
    other.mMessenger = VK_NULL_HANDLE;
  }

  return *this;
}

DebugMessenger::~DebugMessenger() noexcept
{
  destroy();
}

void DebugMessenger::destroy() noexcept
{
  if (mFunctions.vkDestroyDebugUtilsMessengerEXT && mMessenger != VK_NULL_HANDLE) {
    mFunctions.vkDestroyDebugUtilsMessengerEXT(mInstance, mMessenger, nullptr);
    mMessenger = VK_NULL_HANDLE;
  }
}

auto DebugMessenger::make(VkInstance instance,
                          const VkDebugUtilsMessengerCreateInfoEXT& messenger_info,
                          VkResult* result) -> DebugMessenger
{
  const auto functions = get_debug_messenger_functions(instance);
  if (!functions.vkCreateDebugUtilsMessengerEXT ||
      !functions.vkDestroyDebugUtilsMessengerEXT) {
    if (result) {
      *result = VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return {};
  }

  VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
  const auto status = functions.vkCreateDebugUtilsMessengerEXT(instance,
                                                               &messenger_info,
                                                               nullptr,
                                                               &messenger);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return DebugMessenger {functions, instance, messenger};
  }

  return {};
}

auto DebugMessenger::make(VkInstance instance,
                          const VkDebugUtilsMessageSeverityFlagsEXT severities,
                          const VkDebugUtilsMessageTypeFlagsEXT types,
                          PFN_vkDebugUtilsMessengerCallbackEXT callback,
                          VkResult* result) -> DebugMessenger
{
  const auto messenger_info =
      make_debug_utils_messenger_info(severities, types, callback);
  return DebugMessenger::make(instance, messenger_info, result);
}

}  // namespace grace
