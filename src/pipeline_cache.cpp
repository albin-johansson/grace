#include "grace/pipeline_cache.hpp"

namespace grace {

auto make_pipeline_cache_info(const void* initial_data,
                              const usize initial_data_size,
                              const VkPipelineCacheCreateFlags flags)
    -> VkPipelineCacheCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
      .pNext = nullptr,
      .flags = flags,
      .initialDataSize = initial_data_size,
      .pInitialData = initial_data,
  };
}

auto PipelineCache::make(VkDevice device,
                         const VkPipelineCacheCreateInfo& cache_info,
                         VkResult* result) -> PipelineCache
{
  VkPipelineCache cache = VK_NULL_HANDLE;
  const auto status = vkCreatePipelineCache(device, &cache_info, nullptr, &cache);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return PipelineCache {device, cache};
  }

  return {};
}

auto PipelineCache::make(VkDevice device,
                         const void* initial_data,
                         const usize initial_data_size,
                         const VkPipelineCacheCreateFlags flags,
                         VkResult* result) -> PipelineCache
{
  const auto cache_info =
      make_pipeline_cache_info(initial_data, initial_data_size, flags);
  return PipelineCache::make(device, cache_info, result);
}

PipelineCache::PipelineCache(VkDevice device, VkPipelineCache cache) noexcept
    : mDevice {device},
      mCache {cache}
{
}

PipelineCache::PipelineCache(PipelineCache&& other) noexcept
    : mDevice {other.mDevice},
      mCache {other.mCache}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mCache = VK_NULL_HANDLE;
}

auto PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mCache = other.mCache;

    other.mDevice = VK_NULL_HANDLE;
    other.mCache = VK_NULL_HANDLE;
  }

  return *this;
}

PipelineCache::~PipelineCache() noexcept
{
  destroy();
}

void PipelineCache::destroy() noexcept
{
  if (mCache != VK_NULL_HANDLE) {
    vkDestroyPipelineCache(mDevice, mCache, nullptr);
    mCache = VK_NULL_HANDLE;
  }
}

auto PipelineCache::get_size(VkResult* result) const -> usize
{
  usize data_size = 0;
  const auto status = vkGetPipelineCacheData(mDevice, mCache, &data_size, nullptr);

  if (result) {
    *result = status;
  }

  return data_size;
}

auto PipelineCache::get_data(VkResult* result) const -> std::vector<std::byte>
{
  std::vector<std::byte> bytes;

  auto data_size = get_size(result);
  if (data_size == 0) {
    return bytes;
  }

  bytes.resize(data_size);
  const auto status = vkGetPipelineCacheData(mDevice, mCache, &data_size, bytes.data());

  if (result) {
    *result = status;
  }

  return bytes;
}

}  // namespace grace
