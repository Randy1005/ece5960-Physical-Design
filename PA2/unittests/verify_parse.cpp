#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest.h>
#include <floorplanner/floorplanner.hpp>

// Unit test: parsing functionality
TEST_CASE("Parsing" * doctest::timeout(300)) {
  floorplanner::FloorPlanner fp;
  fp.read_input("0.72", "../../input_pa2/3.block", "../../input_pa2/3.nets");
    
  REQUIRE(fp.alpha == 0.72f);
  REQUIRE(fp.chip_width == 16938);
  REQUIRE(fp.chip_height == 12668);
  REQUIRE(fp.n_blks == 103);
  REQUIRE(fp.n_terms == 3);
  
  // a few id queries to see if
  // _name_to_blk is populated 
  // correctly
  REQUIRE(fp.id_of("bb") == 0);
  REQUIRE(fp.id_of("bk4a") == 12);
  REQUIRE(fp.id_of("bk2b") == 15);
  REQUIRE(fp.id_of("P9") == 105);
  REQUIRE(fp.id_of("VDD") == 104);
}

