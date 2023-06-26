#pragma once

#include "grace/common.hpp"

namespace grace {

struct Version final {
  uint32 major {};
  uint32 minor {};
  uint32 patch {};
};

struct ApiVersion final {
  uint32 major {};
  uint32 minor {};
};

[[nodiscard]] auto to_u32(const Version& version) -> uint32;

[[nodiscard]] auto to_u32(const ApiVersion& version) -> uint32;

}  // namespace grace
