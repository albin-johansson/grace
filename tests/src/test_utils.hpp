#pragma once

#include <vulkan/vulkan.h>

#include "grace/allocator.hpp"
#include "grace/device.hpp"
#include "grace/extras/window.hpp"
#include "grace/instance.hpp"
#include "grace/surface.hpp"

namespace grace {

struct TestContext final {
  Window window;
  Instance instance;
  Surface surface;
  VkPhysicalDevice gpu {VK_NULL_HANDLE};
  Device device;
  UniqueAllocator allocator;
};

[[nodiscard]] auto make_test_context() -> TestContext;

}  // namespace grace
