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

// Tests for extension matching
TEST_CASE( "Extension matching works", "[helper][is_interesting]" ) {
   SECTION("Find the good ones") {
      CHECK(is_interesting("test.avi") == true);
      CHECK(is_interesting("test.MkV") == true);
   }
   SECTION("Reject the bad ones") {
      CHECK(is_interesting("avi.txt") == false);
      CHECK(is_interesting("MkV.avi.mov.bak") == false);
   }
}

// Test for hash computation
TEST_CASE( "Hash computation", "[helper][hash]" ) {
   SECTION("Idempotent") {
      uint64_t osdb1, osdb2;
      uint32_t m1, m2;
      FILE* f = NULL;

      f = fopen(fixtures "/dirs/a_first", "r");
      compute_hash(osdb1, m1, f);
      compute_hash(osdb2, m2, f);
      fclose(f);
      CHECK(osdb1 == osdb2);
      CHECK(m1    == m2);

      f = fopen(fixtures "/dirs/a_first", "r");
      compute_hash(osdb2, m2, f); 
      fclose(f);
      CHECK(osdb1 == osdb2);
      CHECK(m1    == m2);\

      f = fopen(fixtures "/dirs/m_middle", "r");
      compute_hash(osdb2, m2, f); 
      fclose(f);
      CHECK(osdb1 == osdb2);
      CHECK(m1    == m2);
   }
}
