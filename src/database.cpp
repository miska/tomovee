/**
 * \brief Implementation of database abstraction
 */

#include <memory>
#include <sqlite3.h>

#include "structures.hpp"
#include "database.hpp"

Database* instance = nullptr;

struct DBFile {
   sqlite3_int64 id;
   unique_ptr<File> file;
   
   bool was_found() { return (bool)file; }
   
   DBFile(sqlite3_int64 id, File* file) : id(id), file(file) {}
   DBFile() : id(-1),  file(nullptr) {}
};

class SqliteDatabase: public Database { 
   string filename;
   sqlite3 *dbHandle;
   
public:
   SqliteDatabase(const string& filename):filename(filename),dbHandle(0) {
      auto rc = sqlite3_open(filename.c_str(), &dbHandle);
      if(rc != SQLITE_OK) {
         dbHandle = 0; // defensive
         throw "Cannot open database";
      }
      instance = this;
      create_schema();
   }
   
   ~SqliteDatabase() override {
      if(dbHandle != 0){
         sqlite3_close(dbHandle);
      }
   }
   
   //! Return all files from DB
   vector<File> get_files() override;
   
   //! Changes paths of given files.
   void update_files(const vector<File>& files) override;

   //! Changes paths of given file.
   void update_file(const File& file) override;

private:
   //! Insert file record in DB (without paths)
   // returns id of the record
   sqlite3_int64 add_file(const File& file);
   
   void create_schema();
   
   
   DBFile get_file_by_hash(const string& hash);
   vector<Path> get_paths_by_fileid(sqlite3_int64 id);

   void add_paths(sqlite3_int64 id, const File&);
   void remove_paths(sqlite3_int64 id);
};


void SqliteDatabase::create_schema() {

const char * sql = 
"CREATE TABLE IF NOT EXISTS file (id INTEGER PRIMARY KEY, hash TEXT UNIQUE, size INTEGER);"
"CREATE TABLE IF NOT EXISTS path (storage TEXT, path TEXT NOT NULL, file_id INTEGER, FOREIGN KEY(file_id) REFERENCES file(id));";

   sqlite3_exec(dbHandle, sql, nullptr, nullptr, nullptr);
}

vector<Path> SqliteDatabase::get_paths_by_fileid(sqlite3_int64 id) {
   sqlite3_stmt * statement;
   int rc = 0;
   
   rc = sqlite3_prepare(dbHandle, "SELECT storage, path FROM path WHERE file_id = :id", -1, &statement, 0);
   rc = sqlite3_bind_int64(statement, 1,  id);  
   
   vector<Path> result;
   for(rc = sqlite3_step(statement); rc == SQLITE_ROW; rc = sqlite3_step(statement)) {
      auto storage = string((const char*)sqlite3_column_text(statement, 0));
      auto path = string((const char*)sqlite3_column_text(statement, 1));
      
      result.push_back(Path(storage, path));
   }
   
   return result;
}

DBFile SqliteDatabase::get_file_by_hash(const string& hash) {
   sqlite3_stmt * statement;
   int rc = 0;
   
   rc = sqlite3_prepare(dbHandle, "SELECT id, size FROM file WHERE hash = :hash", -1, &statement, 0);
   rc = sqlite3_bind_text(statement, 1,  hash.c_str(),  -1, 0);   
   rc = sqlite3_step(statement);
   if (rc == SQLITE_ROW) {
      auto id = sqlite3_column_int64(statement, 0);
      auto size = sqlite3_column_int64(statement, 1);
      
      auto paths = get_paths_by_fileid(id);
   
      sqlite3_finalize(statement);
      return DBFile(id, new File(hash, size, paths));

   } else {
      sqlite3_finalize(statement);
      return DBFile();
   }
}

void SqliteDatabase::add_paths(sqlite3_int64 id, const File& file) {
   int rc;
   sqlite3_stmt * statement;
   
   rc = sqlite3_prepare(dbHandle, "INSERT INTO path(storage, path, file_id) VALUES (:storage, :path, :file_id)", -1, &statement, 0);
   for (auto path : file.get_paths()) {
      sqlite3_bind_text(statement, 1, path.storage.c_str(),-1, SQLITE_STATIC);
      sqlite3_bind_text(statement, 2, path.path.c_str(),-1, SQLITE_STATIC);
      sqlite3_bind_int64(statement,  3,  id);
   
      rc = sqlite3_step(statement);
      rc = sqlite3_reset(statement);
   }

   rc = sqlite3_finalize(statement);
}

void SqliteDatabase::remove_paths(sqlite3_int64 id) {
   int rc;
   sqlite3_stmt * statement;
   
   rc = sqlite3_prepare(dbHandle, "DELETE FROM path WHERE file_id = :fileid", -1, &statement, 0);
   sqlite3_bind_int64(statement, 1, id);
   rc = sqlite3_step(statement);
   
   rc = sqlite3_finalize(statement);
}

sqlite3_int64 SqliteDatabase::add_file(const File& file) {
   sqlite3_stmt * statement;
   int rc = 0;
   
   rc = sqlite3_prepare(dbHandle, "INSERT INTO file(hash, size) VALUES (:hash,:size)", -1, &statement, 0);
   
   rc = sqlite3_bind_text(statement, 1, file.get_hash().c_str(), -1, SQLITE_STATIC);   
   rc = sqlite3_bind_int64(statement, 2, file.get_size());
   rc = sqlite3_step(statement);
   rc = sqlite3_finalize(statement);

   sqlite3_int64 id = sqlite3_last_insert_rowid(dbHandle);   
   return id;
}


void SqliteDatabase::update_file(const File& file){
   DBFile db_file = get_file_by_hash(file.get_hash());
   if (db_file.was_found()) {
      // delete old paths
      remove_paths(db_file.id);
      // add new paths
      add_paths(db_file.id, file);

   } else {
      auto id = add_file(file);
      add_paths(id, file);
   }
}

void SqliteDatabase::update_files(const vector<File>& files){
   
   for(File file : files) {
      update_file(file);
   }
}


vector<File> SqliteDatabase::get_files() {
   vector<File> result;
   sqlite3_stmt * statement;
   int rc = 0;
   
   
   //TODO: not very effective
   rc = sqlite3_prepare(dbHandle, "SELECT hash FROM file", -1, &statement, 0);
   while ((rc = sqlite3_step(statement)) ==  SQLITE_ROW) {
      string hash = string((const char*)sqlite3_column_text(statement, 0));
      
      DBFile dbfile = get_file_by_hash(hash);
      result.push_back(*dbfile.file);
   }
   rc = sqlite3_finalize(statement);
   
   return result;
}

Database* Database::get_instance() {
   if(instance == nullptr)
      throw "No database open!";
   return instance;
}

Database * open_sqlite_db(const string& filename) {
   return new SqliteDatabase(filename);
}
