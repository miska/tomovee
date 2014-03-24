#include <stdio.h>
#include "catch/catch.hpp"
#include "database.hpp"

static vector<File> getFiles() {
   vector<File> result = vector<File>();

   result.push_back(File("tests/fixtures/dirs/a_first","dummy_storage"));
   result.push_back(File("tests/fixtures/dirs/x_last","dummy_storage"));
      
   return result;
}

// so we can compare vectors of files
bool operator==(const File& a, const File& b) {
   return a.looks_same(b) && a.get_paths()==b.get_paths();
}

static const char * db_filename = "tomovee.sqlite";

TEST_CASE("Database layer", "[DB]") {

   SECTION("Empty database gives no files") {
      remove(db_filename);
      unique_ptr<Database> db(open_sqlite_db(db_filename));      
      vector<File> files = db->get_files();
      CHECK(files.size() == 0);
   }

   SECTION("DB Fillings") {
      remove(db_filename);
      unique_ptr<Database> db(open_sqlite_db(db_filename));
      
      auto oldfiles = getFiles();
      db->update_files(oldfiles);
      auto newfiles = db->get_files();
      
      CHECK(oldfiles == newfiles);
   }
}
