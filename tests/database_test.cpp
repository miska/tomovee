#include "catch/catch.hpp"
#include "database.hpp"

TEST_CASE( "Database abstraction works", "[DB]" ) {
   SECTION("Test singleton") {
      Database* test = DB;
      CHECK(test == DB);
   }
}
