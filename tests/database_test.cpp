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
   remove(db_filename);
   unique_ptr<Database> db(open_sqlite_db(db_filename));

   SECTION("Pseudo-singleton work") {
      CHECK(db.get() == DB);
   }

   SECTION("Empty database gives no files") {
      vector<File> files = DB->get_files();
      CHECK(files.size() == 0);
   }

   SECTION("DB holds what we insert") {
      auto oldfiles = getFiles();
      DB->update_files(oldfiles);
      auto newfiles = DB->get_files();
      
      CHECK(oldfiles == newfiles);
   }

   remove(db_filename);
}
