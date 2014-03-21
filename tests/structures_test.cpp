#include "catch/catch.hpp"
#include "helper.hpp"
#include "structures.hpp"
#include <string.h>

#define fixtures "tests/fixtures"

TEST_CASE( "File works", "[structures][file]" ) {
   File test_a(fixtures "/dirs/a_first",  "test");

   SECTION("Verify metadata are read correctly") {
      // Size is read right
      CHECK(test_a.get_size() == 15);
      // MD5 is read right
      CHECK(test_a.get_hash() == "b078a17c2046ebf0af4fc372df02af41");
   }

   SECTION("Test assimilation") {
      // Assimilation test
      File test_m(fixtures "/dirs/m_middle", "test");
      File test_x(fixtures "/dirs/x_last",   "test");
      test_a.assimilate(test_m);
      test_a.assimilate(test_x);
      // Two paths in a
      CHECK(test_a.paths.size() == 2);
      // None in m
      CHECK(test_m.paths.size() == 0);
      // One in x
      CHECK(test_x.paths.size() == 1);
   }
}

TEST_CASE( "Path works", "[structures][path]" ) {
   Path as("a", "/s");
   Path bs("b", "/s");
   Path at("a", "/t");

   SECTION("Test comparison") {
      CHECK(!((bs == as) || (as == at) || (bs == at)));
      CHECK( ((bs != as) && (as != at) && (bs != at)));
   }

   SECTION("Test assignment") {
      Path as_copy(as);
      CHECK((as == as_copy));
      as_copy = bs;
      CHECK((bs == as_copy));
   }
}

