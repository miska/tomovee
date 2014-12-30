#include "catch/catch.hpp"
#include "helper.hpp"
#include "structures.hpp"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define fixtures "tests/fixtures"


TEST_CASE( "File works", "[structures][file]" ) {
   db_url  = "sqlite:";
   db_url += tmpnam(NULL);
   init_db();

   File test_a(fixtures "/dirs/a_first",  "test");

   SECTION("Verify metadata are read correctly") {
      // Size is read right
      CHECK(test_a.get_size() == 15);
      // MD5 is read right
      CHECK(test_a.get_osdb_hash() == 0xdeae40ded8d8ca9f);
   }

   unlink(db_url.c_str());
}

