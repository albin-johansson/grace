#include "grace/instance.hpp"

#ifdef GRACE_USE_SDL2
#include <SDL2/SDL_vulkan.h>
#endif  // GRACE_USE_SDL2

namespace grace {

void InstanceDeleter::operator()(VkInstance instance) noexcept
{
  vkDestroyInstance(instance, nullptr);
}

auto make_application_info(const char* app_name,
                           const Version& app_version,
                           const ApiVersion& vulkan_version) -> VkApplicationInfo
{
  return VkApplicationInfo {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = app_name,
      .applicationVersion = to_u32(app_version),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
      .apiVersion = to_u32(vulkan_version),
  };
}

auto make_instance_info(const VkApplicationInfo* app_info,
                        const std::vector<const char*>& layers,
                        const std::vector<const char*>& extensions)
    -> VkInstanceCreateInfo
{
  VkInstanceCreateInfo instance_info {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = app_info,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = 0,
      .ppEnabledExtensionNames = nullptr,
  };

#ifdef GRACE_USE_VULKAN_SUBSET
  instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif  // GRACE_USE_VULKAN_SUBSET

  if (!layers.empty()) {
    instance_info.enabledLayerCount = static_cast<uint32>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();
  }

  if (!extensions.empty()) {
    instance_info.enabledExtensionCount = static_cast<uint32>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();
  }

  return instance_info;
}

auto make_instance(const char* app_name,
                   const std::vector<const char*>& layers,
                   const std::vector<const char*>& extensions,
                   const Version& app_version,
                   const ApiVersion& vulkan_version,
                   VkResult* result) -> UniqueInstance
{
  const auto app_info = make_application_info(app_name, app_version, vulkan_version);
  const auto instance_info = make_instance_info(&app_info, layers, extensions);

  VkInstance instance = VK_NULL_HANDLE;
  const auto status = vkCreateInstance(&instance_info, nullptr, &instance);

  if (result) {
    *result = status;
  }

  return UniqueInstance {instance};
}

#ifdef GRACE_USE_SDL2

auto get_required_instance_extensions(SDL_Window* window) -> std::vector<const char*>
{
  uint32 extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

  std::vector<const char*> extensions;
  extensions.resize(extension_count);

  SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data());

#ifdef GRACE_USE_VULKAN_SUBSET
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // GRACE_USE_VULKAN_SUBSET

  return extensions;
}

#endif  // GRACE_USE_SDL2

}  // namespace grace
