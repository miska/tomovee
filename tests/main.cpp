#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()

#include "catch/catch.hpp"
#include "helper.hpp"
#include "structures.hpp"
#include <string.h>

#define fixtures "tests/fixtures"

// Tests for find
TEST_CASE( "Find works", "[helper][find]" ) {
   string test;

   // Find last file
   SECTION("Find the last file") {
      find(fixtures, [&test](const char* file) {
         string tmp = file;
         if(tmp>test)
            test = tmp;
      } );

      CHECK(test == string(fixtures "/dirs/x_last"));
   }

   // Find all files without recursing too deep
   SECTION("Iterate/count files without descending too far.") {
      find(fixtures, [&test](const char* ) { test += ";"; },
         [](const char*     ) { return true; },
         [](const char* file) { return strcmp(fixtures "/dirs",file)==0 ||
                                       strcmp(fixtures        ,file)==0; } );

      CHECK(test == string(";;;"));
   }

   // Find file with path containing "inside"
   SECTION("Find file with \"inside\" in name.") {
      find(fixtures, [&test](const char* file) { test = string(file); },
         [](const char* file) -> bool { 
            return strstr(file, "inside") != NULL;
         } );

      CHECK(test == string(fixtures "/dirs/inside/inside_file"));
   }
}

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
