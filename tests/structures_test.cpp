#include "catch/catch.hpp"
#include "helper.hpp"
#include "structures.hpp"
#include <string.h>

#define fixtures "tests/fixtures"


TEST_CASE( "File works", "[structures][file]" ) {
   db_url = "sqlite:test.sqlite";
   init_db();
   File test_a(fixtures "/dirs/a_first",  "test");

   SECTION("Verify metadata are read correctly") {
      // Size is read right
      CHECK(test_a.get_size() == 15);
      // MD5 is read right
      CHECK(test_a.get_osdb_hash() == 0xdeae40ded8d8ca9f);
   }

   SECTION("Test assimilation") {
      // Assimilation test
      File test_m(fixtures "/dirs/m_middle", "test");
      File test_x(fixtures "/dirs/x_last",   "test");
      test_a.assimilate(test_m);
      test_a.assimilate(test_x);
      // Two paths in a
      CHECK(test_a.get_paths().size() == 2);
      // None in m
      CHECK(test_m.get_paths().size() == 0);
      // One in x
      CHECK(test_x.get_paths().size() == 1);
   }
}

