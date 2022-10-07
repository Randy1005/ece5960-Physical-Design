#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest.h>
#include <floorplanner/floorplanner.hpp>

// Unit test: example usage
TEST_CASE("Example" * doctest::timeout(300)) {
  int a = 1 + 3;
  REQUIRE(a == 4);
}

