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

}  // namespace grace
