#pragma once

#include <cstddef>  // byte
#include <vector>   // vector

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

[[nodiscard]] auto make_pipeline_cache_info(const void* initial_data = nullptr,
                                            usize initial_data_size = 0,
                                            VkPipelineCacheCreateFlags flags = 0)
    -> VkPipelineCacheCreateInfo;

class PipelineCache final {
 public:
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkPipelineCacheCreateInfo& cache_info,
                                 VkResult* result = nullptr) -> PipelineCache;

  [[nodiscard]] static auto make(VkDevice device,
                                 const void* initial_data = nullptr,
                                 usize initial_data_size = 0,
                                 VkPipelineCacheCreateFlags flags = 0,
                                 VkResult* result = nullptr) -> PipelineCache;

  PipelineCache() noexcept = default;

  PipelineCache(VkDevice device, VkPipelineCache cache) noexcept;

  PipelineCache(PipelineCache&& other) noexcept;
  PipelineCache(const PipelineCache& other) = delete;

  auto operator=(PipelineCache&& other) noexcept -> PipelineCache&;
  auto operator=(const PipelineCache& other) -> PipelineCache& = delete;

  ~PipelineCache() noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get_size(VkResult* result = nullptr) const -> usize;

  [[nodiscard]] auto get_data(VkResult* result = nullptr) const -> std::vector<std::byte>;

  [[nodiscard]] auto get() noexcept -> VkPipelineCache { return mCache; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkPipelineCache() noexcept { return mCache; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mCache != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkPipelineCache mCache {VK_NULL_HANDLE};
};

}  // namespace grace
