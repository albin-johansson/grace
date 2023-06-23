#include "grace/instance.hpp"

#include <doctest/doctest.h>
#include <vulkan/vulkan.h>

TEST_SUITE("Instance")
{
  TEST_CASE("make_application_info")
  {
    const auto app_info = grace::make_application_info("Tests", {1, 2, 3}, {1, 2});

    CHECK(app_info.sType == VK_STRUCTURE_TYPE_APPLICATION_INFO);
    CHECK(app_info.pNext == nullptr);
    CHECK(app_info.pApplicationName == doctest::String("Tests"));
    CHECK(app_info.pEngineName == doctest::String("No Engine"));
    CHECK(app_info.apiVersion == VK_API_VERSION_1_2);
    CHECK(app_info.applicationVersion == VK_MAKE_VERSION(1, 2, 3));
    CHECK(app_info.engineVersion == VK_MAKE_VERSION(0, 1, 0));
  }

  TEST_CASE("make_instance_info")
  {
    const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const auto app_info = grace::make_application_info("Tests", {1, 2, 3}, {1, 2});
    const auto instance_info = grace::make_instance_info(&app_info, layers, extensions);

    CHECK(instance_info.sType == VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
    CHECK(instance_info.pNext == nullptr);
    CHECK(instance_info.pApplicationInfo == &app_info);
    CHECK(instance_info.enabledLayerCount == 1);
    CHECK(instance_info.ppEnabledLayerNames == layers.data());
    CHECK(instance_info.enabledExtensionCount == 1);
    CHECK(instance_info.ppEnabledExtensionNames == extensions.data());

#ifdef GRACE_USE_VULKAN_SUBSET
    CHECK(instance_info.flags == VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR);
#else
    CHECK(instance_info.flags == 0);
#endif  // GRACE_USE_VULKAN_SUBSET
  }
}
