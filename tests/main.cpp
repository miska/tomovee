#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch/catch.hpp"
#include "helper.hpp"
#include "structures.hpp"
#include <string.h>

#define fixtures "tests/fixtures"

// Tests for find
TEST_CASE( "Find works", "[helper/find]" ) {
   string test;

   // Find last file
   find(fixtures, [&test](const char* file) { test = string(file); } );

   REQUIRE(test == string(fixtures "/dirs/x_last"));

   // Find all files without recursing too deep
   test = "";
   find(fixtures, [&test](const char* file) { test += string(file) + ";"; },
      [](const char*     ) { return true; },
      [](const char* file) { return strcmp(fixtures "/dirs",file)==0 ||
                                    strcmp(fixtures        ,file)==0; } );

   REQUIRE(test == string(fixtures "/dirs/a_first;"
                          fixtures "/dirs/m_middle;"
                          fixtures "/dirs/x_last;"));

   // Find file with path containing "inside"
   test = "";
   find(fixtures, [&test](const char* file) { test = string(file); },
      [](const char* file) -> bool { 
         return strstr(file, "inside") != NULL;
      } );

   REQUIRE(test == string(fixtures "/dirs/inside/inside_file"));
}

TEST_CASE( "File works", "[structures/file]" ) {
   File test_a(fixtures "/dirs/a_first",  "test");
   File test_m(fixtures "/dirs/m_middle", "test");
   File test_x(fixtures "/dirs/x_last",   "test");
   // Size is read right
   REQUIRE(test_a.get_size() == 15);
   // MD5 is read right
   REQUIRE(test_a.get_hash() == "b078a17c2046ebf0af4fc372df02af41");

   // Assimilation test
   test_a.assimilate(test_m);
   test_a.assimilate(test_x);
   // Two paths in a
   REQUIRE(test_a.paths.size() == 2);
   // None in m
   REQUIRE(test_m.paths.size() == 0);
   // One in x
   REQUIRE(test_x.paths.size() == 1);
}
