#pragma once

#include <cstddef>    // size_t
#include <cstdint>    // uint32_t, uint64_t
#include <optional>   // optional
#include <stdexcept>  // runtime_error
#include <utility>    // move

#include <vulkan/vulkan.h>

namespace grace {

using usize = std::size_t;

using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

/**
 * Used as the return type by factory functions.
 *
 * \details When dealing with instances of this type, you should either verify that the
 *          object member is not empty or that the the status member is `VK_SUCCESS` to
 *          determine that the operation was successful. Alternatively, you can use the
 *          `has_value` member function.
 *
 * \tparam T a library Vulkan wrapper type.
 */
template <typename T>
struct Result final {
  std::optional<T> obj;  ///< A potentially empty object.
  VkResult status {};    ///< The status code from the operation that created `obj`.

  /// Indicates whether the instance holds a non-empty object and a non-error status code.
  [[nodiscard]] auto has_value() const noexcept -> bool
  {
    return obj.has_value() && status == VK_SUCCESS;
  }

  /// Extracts the associated value, or raises an exception if there is none.
  [[nodiscard]] auto unwrap() -> T
  {
    if (has_value()) {
      return std::move(*obj);
    }

    throw std::runtime_error {"Tried to unwrap empty result object"};
  }
};

}  // namespace grace
