/**
 * \brief Implementation of objects used in the application
 */

#include "structures.hpp"
#include "helper.hpp"

#include <tntdb/connection.h>
#include <tntdb/connect.h>
#include <tntdb/row.h>

void File::update_meta(const char* file) {
   std::string a,s;
   get_movie_info(file, length, width, height, a, s);
   if(audios.empty())
      audios = a;
   if(subtitles.empty())
      subtitles = s;
   tntdb::Connection conn = tntdb::connectCached(db_url);
   auto smt = conn.prepareCached("UPDATE files SET audios = :audio, "
                                 "subtitles = :sub, width = :w, height = :h, "
                                 "length = :l WHERE id = :id");
   smt.set("audio", audios).set("sub", subtitles).set("w", width).
       set("h", height).set("l", length).set("id", db_id).execute();
}

//! Update last checked time
void Path::touch() {
   checked = time(NULL);
   tntdb::Connection conn = tntdb::connectCached(db_url);
   auto smt = conn.prepareCached("UPDATE paths SET checked = :checked "
                                 "WHERE id = :id");
   smt.set("checked", checked).set("id", db_id).execute();
}

Path::Path(const string& st, const string& pth, uint64_t parent) {
   { // Database operations block start

   tntdb::Connection conn = tntdb::connectCached(db_url);
   tntdb::Statement smt;
   tntdb::Row row;

   // Add into database if doesn't exists
   smt = conn.prepareCached("INSERT INTO paths (storage, path, file_id) "
                            "SELECT :storage, :path, :parent WHERE 1 NOT IN "
                            "(SELECT 1 FROM paths WHERE "
                            "storage = :storage AND path = :path AND "
                            "file_id = :parent LIMIT 1)");
   smt.set("storage", st).set("path", pth).set("parent", parent).execute();

   // Get ID from the database
   smt = conn.prepareCached("SELECT id FROM paths WHERE "
                            "storage = :storage AND path = :path AND "
                            "file_id = :parent LIMIT 1");
   row = smt.set("storage", st).set("path", pth).set("parent", parent).
         set("parent", parent).selectRow();
   row[0].get(db_id);

   } // Destroy everything database related, SQLite can handle only one
     // connection at the time, so we have to release it before running
     // touch at the end of constructor

   storage = st;
   path = pth;
   touch();
}

Path& Path::operator=(const Path& other) {
   storage = other.storage;
   path = other.path;
   db_id = other.db_id;
   checked = other.checked;
   return *this;
}

bool Path::operator==(const Path& b) const {
   return ((storage == b.storage) && (path == b.path));
}

File::File(const char* file, const string& storage) {
   added = time(NULL);
   update_info(file);
   paths.push_back(Path(storage, file, db_id));
}

void File::update_info(const char* file) {
   FILE* fl;

   // Set size
   if((fl = fopen(file,"r")) == NULL) return;
   fseek(fl, 0, SEEK_END);
   size = ftell(fl);
   fclose(fl);

   // Calculate hashes
   if((fl = fopen(file,"r")) == NULL) return;
   compute_hash(osdbhash, mhash, fl);
   fclose(fl);

   // Set time
   added = time(NULL);

   { // Database scope
   // Try whether we already have a file like that
   tntdb::Connection conn = tntdb::connectCached(db_url);
   tntdb::Statement smt;
   tntdb::Row row;

   // Add into database if doesn't exists
   smt = conn.prepareCached("INSERT INTO files (mhash, osdbhash, size, "
                            "added, movie_id, assigned_by) SELECT :m, :osdb, "
                            ":size, :added, 0, :ass WHERE 1 NOT IN "
                            "(SELECT 1 FROM  files WHERE "
                            "mhash = :m AND osdbhash = :osdb AND "
                            "size = :size LIMIT 1)");
   smt.set("m", mhash).set("osdb", osdbhash).set("size", size).
       set("added", added).set("ass", NOT_ASSIGNED).execute();

   // Get some data from the database
   smt = conn.prepareCached("SELECT id, added, audios, subtitles FROM files "
                            "WHERE "
                            "mhash = :m AND osdbhash = :osdb AND "
                            "size = :size LIMIT 1");
   row = smt.set("m", mhash).set("osdb", osdbhash).set("size", size).
         selectRow();

   row[0].get(db_id);
   row[1].get(added);
   row[2].get(audios);
   row[3].get(subtitles);
   } // End of database scope
   update_meta(file);
}

void File::cleanup() {
   tntdb::Connection conn = tntdb::connectCached(db_url);
   auto smt = conn.prepareCached("DELETE FROM files WHERE id NOT IN "
                            "(SELECT file_id FROM paths)");
   smt.execute();
}

void File::set_movie(movie_assigned type, uint64_t movie_id) {
   tntdb::Connection conn = tntdb::connectCached(db_url);
   auto smt = conn.prepareCached("UPDATE files SET movie_id = :mov, "
                                 "assigned_by = :ass WHERE id = :id");
   smt.set("ass", type).set("mov", movie_id).set("id", db_id).execute();
}

void Path::cleanup(time_t older, std::string storage) {
   tntdb::Connection conn = tntdb::connectCached(db_url);
   auto smt = conn.prepareCached("DELETE FROM paths WHERE checked < :ts AND "
                                                         "storage = :st");
   smt.set("ts", older).set("st", storage).execute();
}

Movie::Movie(std::string imdb_id) {
   // Try whether we already have a movie like that
   tntdb::Connection conn = tntdb::connectCached(db_url);
   tntdb::Statement smt;
   tntdb::Row row;

   // Add into database if doesn't exists
   smt = conn.prepareCached("INSERT INTO movies (imdb_id) SELECT :imdb "
                            "WHERE 1 NOT IN (SELECT 1 FROM movies WHERE "
                            "imdb_id = :imdb LIMIT 1)");
   smt.set("imdb", imdb_id).execute();

   // Get ID from the database
   smt = conn.prepareCached("SELECT id FROM movies WHERE imdb_id = :imdb LIMIT 1");
   row = smt.set("imdb", imdb_id).selectRow();

   row[0].get(db_id);
}
