#pragma once

#include <cstddef>    // size_t
#include <cstdint>    // uint32_t, uint64_t
#include <limits>     // numeric_limits

#include <vulkan/vulkan.h>

namespace grace {

using usize = std::size_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int32 = std::int32_t;

inline constexpr uint32 kMaxU32 = std::numeric_limits<uint32>::max();
inline constexpr uint64 kMaxU64 = std::numeric_limits<uint64>::max();

[[nodiscard]] auto to_string(VkResult result) -> const char*;

template <typename Container>
[[nodiscard]] constexpr auto u32_size(const Container& container) -> uint32
{
  return static_cast<uint32>(container.size());
}

template <typename Container>
[[nodiscard]] auto data_or_null(const Container& container)
{
  return !container.empty() ? container.data() : nullptr;
}

}  // namespace grace
