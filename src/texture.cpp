#include "grace/texture.hpp"

namespace grace {

auto Texture::make_2d(VkDevice device,
                      VmaAllocator allocator,
                      const VkExtent2D& extent,
                      const VkImageViewType view_type,
                      const VkFormat format,
                      const VkImageUsageFlags usage,
                      const VkImageAspectFlags view_aspects,
                      const uint32 mip_levels,
                      const VkSampleCountFlagBits samples,
                      VkResult* result) -> Texture
{
  Texture texture;

  texture.image = Image::make(allocator,
                              VK_IMAGE_TYPE_2D,
                              {extent.width, extent.width, 1},
                              format,
                              usage,
                              mip_levels,
                              samples,
                              result);

  if (texture.image) {
    texture.image_view = ImageView::make(device,
                                         texture.image.get(),
                                         view_type,
                                         format,
                                         view_aspects,
                                         mip_levels,
                                         result);
  }

  return texture;
}

void Texture::destroy() noexcept
{
  image_view.destroy();
  image.destroy();
}

}  // namespace grace