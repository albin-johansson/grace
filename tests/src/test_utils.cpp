#include "test_utils.hpp"

#include <vector>  // vector

#include "grace/physical_device.hpp"

namespace grace {

auto make_test_context() -> TestContext
{
  TestContext ctx;
  ctx.window = Window::make("Test", 800, 600);

  const std::vector layers = {"VK_LAYER_KHRONOS_validation"};
  const auto instance_extensions = get_required_instance_extensions(ctx.window);

  std::vector device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                   VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
                                   VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};
#ifdef GRACE_USE_VULKAN_SUBSET
  device_extensions.push_back("VK_KHR_portability_subset");
#endif  // GRACE_USE_VULKAN_SUBSET

  ctx.instance =
      Instance::make("TestApp", layers, instance_extensions, {0, 1, 0}, {1, 2});
  ctx.surface = Surface::make(ctx.window, ctx.instance);

  auto gpu_filter = [](VkPhysicalDevice, VkSurfaceKHR) { return true; };
  auto gpu_rater = [](VkPhysicalDevice) { return 1; };
  ctx.gpu = pick_physical_device(ctx.instance, ctx.surface, gpu_filter, gpu_rater);

  VkPhysicalDeviceDescriptorIndexingFeatures indexing_features = {};
  indexing_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  indexing_features.descriptorBindingPartiallyBound = VK_TRUE;

  const auto device_queue_infos = make_device_queue_infos(ctx.gpu, ctx.surface);
  const auto device_info = make_device_info(device_queue_infos.queues,
                                            layers,
                                            device_extensions,
                                            nullptr,
                                            &indexing_features);

  ctx.device = Device::make(ctx.gpu, device_info);
  ctx.allocator = make_allocator(ctx.instance, ctx.gpu, ctx.device);

  return ctx;
}

}  // namespace grace
