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
