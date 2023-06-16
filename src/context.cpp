#include "grace/context.hpp"

namespace grace {
namespace {

inline constinit VmaAllocator gAllocator = VK_NULL_HANDLE;
inline constinit VkInstance gInstance = VK_NULL_HANDLE;
inline constinit VkDevice gDevice = VK_NULL_HANDLE;
inline constinit VkCommandPool gGraphicsCommandPool = VK_NULL_HANDLE;
inline constinit VkCommandPool gPresentationCommandPool = VK_NULL_HANDLE;

}  // namespace

void SetGlobalAllocator(VmaAllocator allocator) noexcept
{
  gAllocator = allocator;
}

void SetGlobalInstance(VkInstance instance) noexcept
{
  gInstance = instance;
}

void SetGlobalDevice(VkDevice device) noexcept
{
  gDevice = device;
}

void SetGlobalGraphicsCommandPool(VkCommandPool cmd_pool) noexcept
{
  gGraphicsCommandPool = cmd_pool;
}

void SetGlobalPresentationCommandPool(VkCommandPool cmd_pool) noexcept
{
  gPresentationPool = cmd_pool;
}

auto GetGlobalAllocator() noexcept -> VmaAllocator
{
  return gAllocator;
}

auto GetGlobalInstance() noexcept -> VkInstance
{
  return gInstance;
}

auto GetGlobalDevice() noexcept -> VkDevice
{
  return gDevice;
}

auto GetGlobalGraphicsCommandPool() noexcept -> VkCommandPool
{
  return gGraphicsCommandPool;
}

auto GetGlobalPresentationCommandPool() noexcept -> VkCommandPool
{
  return gPresentationCommandPool;
}

}  // namespace grace
