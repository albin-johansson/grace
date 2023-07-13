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

#include "grace/image.hpp"

#include <algorithm>      // max
#include <cassert>        // assert
#include <cmath>          // floor, log2
#include <unordered_map>  // unordered_map

#include "grace/allocator.hpp"
#include "grace/buffer.hpp"
#include "grace/command_pool.hpp"

namespace grace {
namespace {

// Used to determine access flags for layout transitions.
const std::unordered_map<VkImageLayout, VkAccessFlags> kTransitionAccessMap {
    {VK_IMAGE_LAYOUT_UNDEFINED, 0},
    {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT},
    {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT},
    {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT},
    {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
     VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT},
};

// Used to determine pipeline stage flags for layout transitions.
const std::unordered_map<VkImageLayout, VkPipelineStageFlags> kTransitionStageMap {
    {VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT},
    {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},
    {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},
    {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT},
    {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
};

}  // namespace

auto make_image_info(const VkImageType type,
                     const VkExtent3D& extent,
                     const VkFormat format,
                     const VkImageUsageFlags usage,
                     const uint32 mip_levels,
                     const VkSampleCountFlagBits samples) -> VkImageCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .imageType = type,
      .format = format,
      .extent = extent,
      .mipLevels = (samples == VK_SAMPLE_COUNT_1_BIT) ? mip_levels : 1,
      .arrayLayers = 1,
      .samples = samples,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
}

auto get_max_image_mip_levels(const VkExtent3D extent) -> uint32
{
  const auto max_extent = std::max(extent.width, extent.height);
  return 1 + static_cast<uint32>(std::floor(std::log2(max_extent)));
}

void cmd_change_image_layout(VkCommandBuffer cmd_buf,
                             VkImage image,
                             const VkImageLayout old_layout,
                             const VkImageLayout new_layout,
                             const uint32 base_mip_level,
                             const uint32 mip_level_count)
{
  assert(kTransitionAccessMap.contains(old_layout));
  assert(kTransitionAccessMap.contains(new_layout));
  const auto src_access = kTransitionAccessMap.at(old_layout);
  const auto dst_access = kTransitionAccessMap.at(new_layout);

  assert(kTransitionStageMap.contains(old_layout));
  assert(kTransitionStageMap.contains(new_layout));
  const auto src_stage = kTransitionStageMap.at(old_layout);
  const auto dst_stage = kTransitionStageMap.at(new_layout);

  const VkImageMemoryBarrier image_memory_barrier {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = nullptr,
      .srcAccessMask = src_access,
      .dstAccessMask = dst_access,
      .oldLayout = old_layout,
      .newLayout = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image,
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = base_mip_level,
              .levelCount = mip_level_count,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  vkCmdPipelineBarrier(cmd_buf,
                       src_stage,
                       dst_stage,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &image_memory_barrier);
}

void cmd_copy_buffer_to_image(VkCommandBuffer cmd_buf,
                              VkBuffer buffer,
                              VkImage image,
                              const VkExtent3D& image_extent,
                              const VkImageLayout image_layout)
{
  VkBufferImageCopy region = {};

  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = image_extent;

  vkCmdCopyBufferToImage(cmd_buf, buffer, image, image_layout, 1, &region);
}

void ImageInfo::copy_from(const VkImageCreateInfo& image_info)
{
  extent = image_info.extent;
  layout = image_info.initialLayout;
  format = image_info.format;
  samples = image_info.samples;
  mip_levels = image_info.mipLevels;
}

Image::Image(Image&& other) noexcept
    : mAllocator {other.mAllocator},
      mImage {other.mImage},
      mAllocation {other.mAllocation},
      mInfo {other.mInfo}
{
  other.mAllocator = VK_NULL_HANDLE;
  other.mImage = VK_NULL_HANDLE;
  other.mAllocation = VK_NULL_HANDLE;
}

auto Image::operator=(Image&& other) noexcept -> Image&
{
  if (this != &other) {
    destroy();

    mAllocator = other.mAllocator;
    mImage = other.mImage;
    mAllocation = other.mAllocation;
    mInfo = other.mInfo;

    other.mAllocator = VK_NULL_HANDLE;
    other.mImage = VK_NULL_HANDLE;
    other.mAllocation = VK_NULL_HANDLE;
  }

  return *this;
}

Image::~Image() noexcept
{
  destroy();
}

void Image::destroy() noexcept
{
  if (mImage != VK_NULL_HANDLE) {
    vmaDestroyImage(mAllocator, mImage, mAllocation);
    mImage = VK_NULL_HANDLE;
  }
}

auto Image::make(VmaAllocator allocator,
                 const VkImageCreateInfo& image_info,
                 const VmaAllocationCreateInfo& allocation_info,
                 VkResult* result) -> Image
{
  Image image;
  image.mAllocator = allocator;

  const auto status = vmaCreateImage(allocator,
                                     &image_info,
                                     &allocation_info,
                                     &image.mImage,
                                     &image.mAllocation,
                                     nullptr);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    image.mInfo.copy_from(image_info);
    return image;
  }

  return {};
}

auto Image::make(VmaAllocator allocator,
                 const VkImageType type,
                 const VkExtent3D& extent,
                 const VkFormat format,
                 const VkImageUsageFlags usage,
                 const uint32 mip_levels,
                 const VkSampleCountFlagBits samples,
                 VkResult* result) -> Image
{
  const auto image_info =
      make_image_info(type, extent, format, usage, mip_levels, samples);
  const auto allocation_info = make_allocation_info(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                    0,
                                                    0,
                                                    VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
  return make(allocator, image_info, allocation_info, result);
}

auto Image::set_data(const CommandContext& ctx,
                     VmaAllocator allocator,
                     const void* data,
                     const uint64 data_size) -> VkResult
{
  VkResult result = VK_SUCCESS;

  auto staging_buffer = Buffer::for_staging(allocator, data_size, 0, &result);
  if (!staging_buffer) {
    return result;
  }

  result = staging_buffer.set_data(data, data_size);
  if (result != VK_SUCCESS) {
    return result;
  }

  // Optimize layout for the buffer transfer, and copy data from staging buffer.
  change_layout(ctx, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copy_buffer(ctx, staging_buffer.get());

  // Generate mipmaps, which will automatically change layout of all image levels
  generate_mipmaps(ctx);

  assert(mInfo.layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  return result;
}

void Image::change_layout(const CommandContext& ctx, const VkImageLayout new_layout)
{
  execute_now(ctx, [this, new_layout](VkCommandBuffer cmd_buf) {
    cmd_change_image_layout(cmd_buf,
                            mImage,
                            mInfo.layout,
                            new_layout,
                            0,
                            mInfo.mip_levels);
    mInfo.layout = new_layout;
  });
}

void Image::copy_buffer(const CommandContext& ctx, VkBuffer buffer)
{
  execute_now(ctx, [this, buffer](VkCommandBuffer cmd_buf) {
    cmd_copy_buffer_to_image(cmd_buf, buffer, mImage, mInfo.extent, mInfo.layout);
  });
}

void Image::generate_mipmaps(const CommandContext& ctx)
{
  assert(mInfo.samples | VK_SAMPLE_COUNT_1_BIT);
  assert(mInfo.layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  execute_now(ctx, [this](VkCommandBuffer cmd_buf) {
    auto mip_width = static_cast<int32>(mInfo.extent.width);
    auto mip_height = static_cast<int32>(mInfo.extent.height);

    for (uint32 mip_level = 1; mip_level < mInfo.mip_levels; ++mip_level) {
      const uint32 base_mip_level = mip_level - 1;

      cmd_change_image_layout(cmd_buf,
                              mImage,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              base_mip_level,
                              1);

      VkImageBlit blit {};
      blit.srcOffsets[0] = {0, 0, 0};
      blit.srcOffsets[1] = {mip_width, mip_height, 1};

      blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.srcSubresource.mipLevel = base_mip_level;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount = 1;

      blit.dstOffsets[0] = {0, 0, 0};
      blit.dstOffsets[1] = {(mip_width > 1) ? (mip_width / 2) : 1,
                            (mip_height > 1) ? (mip_height / 2) : 1,
                            1};

      blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.dstSubresource.mipLevel = mip_level;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount = 1;

      vkCmdBlitImage(cmd_buf,
                     mImage,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     mImage,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     1,
                     &blit,
                     VK_FILTER_LINEAR);

      cmd_change_image_layout(cmd_buf,
                              mImage,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              base_mip_level,
                              1);

      if (mip_width > 1) {
        mip_width /= 2;
      }

      if (mip_height > 1) {
        mip_height /= 2;
      }
    }

    // Transitions the last mipmap image to the optimal shader read layout
    cmd_change_image_layout(cmd_buf,
                            mImage,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            mInfo.mip_levels - 1,
                            1);

    mInfo.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  });
}

}  // namespace grace
