#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"
#include "helper.hpp"
#include <string.h>

#define fixtures "tests/fixtures"

TEST_CASE( "Find works", "[helper/find]" ) {
   string test;

   find(fixtures, [&test](const char* file) { test = string(file); } );

   REQUIRE(test == string(fixtures "/dirs/x_last"));

   test = "";
   find(fixtures, [&test](const char* file) { test += string(file) + ";"; },
      [](const char* file) { return true; },
      [](const char* file) { return strcmp(fixtures "/dirs",file)==0 ||
                                    strcmp(fixtures        ,file)==0; } );

   REQUIRE(test == string(fixtures "/dirs/a_first;" fixtures "/dirs/x_last;"));

   test = "";
   find(fixtures, [&test](const char* file) { test = string(file); },
      [](const char* file) -> bool { 
         return strstr(file, "inside") != NULL;
      } );

   REQUIRE(test == string(fixtures "/dirs/inside/inside_file"));
}

