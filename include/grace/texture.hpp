#pragma once

#include <vulkan/vulkan.h>

#include "common.hpp"
#include "image.hpp"
#include "image_view.hpp"

namespace grace {

struct Texture final {
  Image image;
  ImageView image_view;

  [[nodiscard]] static auto make_2d(VkDevice device,
                                    VmaAllocator allocator,
                                    const VkExtent2D& extent,
                                    VkImageViewType view_type,
                                    VkFormat format,
                                    VkImageUsageFlags usage,
                                    VkImageAspectFlags view_aspects,
                                    uint32 mip_levels = 1,
                                    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                                    VkResult* result = nullptr) -> Texture;

  void destroy() noexcept;

  [[nodiscard]] explicit operator bool() const noexcept { return image && image_view; }
};

}  // namespace grace
