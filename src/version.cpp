#include "grace/version.hpp"

namespace grace {

auto to_u32(const Version& version) -> uint32
{
  return VK_MAKE_VERSION(version.major, version.minor, version.patch);
}

auto to_u32(const ApiVersion& version) -> uint32
{
  return VK_MAKE_API_VERSION(0, version.major, version.minor, 0);
}

}  // namespace grace
