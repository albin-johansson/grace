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

#pragma once

#include <cstddef>  // size_t
#include <cstdint>  // uint32_t, uint64_t
#include <limits>   // numeric_limits

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
