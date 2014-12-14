/**
 * \brief Implementation of objects used in the application
 */

#include "structures.hpp"
#include "helper.hpp"
#include "polarssl/md5.h"

#include <tntdb/connection.h>
#include <tntdb/connect.h>
#include <tntdb/row.h>

//! Update checked time
void File::touch() {
   checked = time(NULL);
   save();
}

//! Update times
void File::save() {
   tntdb::Connection conn = tntdb::connectCached(db_url);
   auto smt = conn.prepareCached("UPDATE files SET checked = :checked "
                                 "WHERE id = :id");
   smt.set("checked", checked).set("id", db_id).execute();
}

//! Function to compute Open Subtitles DB hash
uint64_t compute_osbd_hash(FILE * handle) {
        uint64_t hash, fsize;

        fseek(handle, 0, SEEK_END);
        fsize = ftell(handle);
        fseek(handle, 0, SEEK_SET);

        hash = fsize;

        for(uint64_t tmp = 0, i = 0;
            i < 65536/sizeof(tmp) && fread((char*)&tmp, sizeof(tmp), 1, handle);
            hash += tmp, i++);

        fseek(handle, (uint64_t)max((uint64_t)0, fsize - 65536), SEEK_SET);

        for(uint64_t tmp = 0, i = 0;
            i < 65536/sizeof(tmp) && fread((char*)&tmp, sizeof(tmp), 1, handle);
            hash += tmp, i++);
        
        return hash;
}

Path::Path(const string& st, const string& pth, uint64_t parent) {
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

   storage = st;
   path = pth;
}

Path& Path::operator=(const Path& other) {
   storage = other.storage;
   path = other.path;
   db_id = other.db_id;
   return *this;
}

bool Path::operator==(const Path& b) const {
   return ((storage == b.storage) && (path == b.path));
}

File::File(const char* file, const string& storage) {
   added = checked = time(NULL);
   update_info(file);
   paths.push_back(Path(storage, file, db_id));
}

void File::update_info(const char* file) {
   unsigned char sum[16];
   char buff[33];
   char* buff_ptr = buff;
   FILE* fl;

   loaded = false;

   // Set size
   if((fl = fopen(file,"r")) == NULL) return;
   fseek(fl, 0, SEEK_END);
   size = ftell(fl);
   fclose(fl);

   // Calculate md5
   if(md5_file(file, sum) == 0) {
      for(int i=0; i<16; i++) {
         snprintf(buff_ptr, 3, "%02x", sum[i]);
         buff_ptr+=2;
      }
      buff[32] = 0;
      md5hash = buff;
   }

   // Calculate OSDB hash
   if((fl = fopen(file,"r")) == NULL) return;
   osdbhash = compute_osbd_hash(fl);
   fclose(fl);

   // Set times
   added = time(NULL);
   checked = time(NULL);

   // Try whether we already have a file like that
   tntdb::Connection conn = tntdb::connectCached(db_url);
   tntdb::Row row;
   tntdb::Statement smt;

   // Add into database if doesn't exists
   smt = conn.prepareCached("INSERT INTO files (md5hash, osdbhash, size, "
                            "added, checked) SELECT :md5, :osdb, "
                            ":size, :added, :checked WHERE 1 NOT IN "
                            "(SELECT 1 FROM  files WHERE "
                            "md5hash = :md5 AND osdbhash = :osdb AND "
                            "size = :size LIMIT 1)");
   smt.set("md5", md5hash).set("osdb", osdbhash).set("size", size).
       set("added", added).set("checked", checked).execute();

   // Get ID from the database
   smt = conn.prepareCached("SELECT id, added, checked FROM files WHERE "
                            "md5hash = :md5 AND osdbhash = :osdb AND "
                            "size = :size LIMIT 1");
   row = smt.set("md5", md5hash).set("osdb", osdbhash).set("size", size).
         selectRow();

   row[0].get(db_id);
   row[1].get(added);
   row[2].get(checked);

   loaded = true;
}

void File::assimilate(File& other) {
   if(looks_same(other)) {
      for(auto i : other.paths) {
         paths.push_back(Path(i));
      }
      other.paths.erase(other.paths.begin(), other.paths.end());
      added = min(added, other.added);
      checked = max(checked, other.checked);
   }
}
