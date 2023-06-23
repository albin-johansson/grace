#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "grace/extras/sdl.hpp"

int main(int argc, char* argv[])
{
  const grace::SDL sdl;

  doctest::Context context {argc, argv};
  const auto res = context.run();

  if (context.shouldExit()) {
    return res;
  }

  return 0;
}