#include "grace/buffer.hpp"

#include <doctest/doctest.h>

TEST_SUITE("Buffers")
{
  TEST_CASE("make_buffer_info")
  {
    const auto buffer_info =
        grace::make_buffer_info(1'000, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    CHECK(buffer_info.sType == VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
    CHECK(buffer_info.size == 1'000);
    CHECK(buffer_info.usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    CHECK(buffer_info.pNext == nullptr);
    CHECK(buffer_info.flags == 0);
    CHECK(buffer_info.pQueueFamilyIndices == nullptr);
    CHECK(buffer_info.sharingMode == VK_SHARING_MODE_EXCLUSIVE);
  }
}