#include "catch/catch.hpp"
#include "database.hpp"

TEST_CASE( "Database abstraction works", "[DB]" ) {
   SECTION("Test singleton") {
      DB* test = DB::get_instance();
      CHECK(test == DB::get_instance());
   }
}
