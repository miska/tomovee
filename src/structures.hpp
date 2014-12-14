/**
 * \brief Definitions of objects used in the application
 */

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <vector>
#include <string>
#include <time.h>

using namespace std;

class File;

//! Basic class to represent a path
class Path {
friend File;
private:
   //! ID in the database
   uint64_t db_id;
protected:
   //! Constructor
   Path(const string& st, const string& pth, uint64_t parent);
public:
   //! On which storage is this path
   string storage;
   //! Path relative to the root of the storage
   string path;
   //! Copy constructor
   Path(const Path& other):  db_id(other.db_id),
                             storage(other.storage),
                             path(other.path) {}

   //! Move constructor
   Path(const Path&& other): db_id(other.db_id),
                             storage(move(other.storage)),
                             path(move(other.path)) {}
   //! Assignment operator
   Path& operator=(const Path& other);
   //! Comparison operator
   bool operator==(const Path& other) const;
   //! Comparison operator
   bool operator!=(const Path& other) { return (!((*this) == other)); }
   //! Delete path from database
   void remove();
};

//! Basic class to represent a file
class File {
   //! ID in the database
   uint64_t db_id;
   //! File md5 hash
   string md5hash;
   //! File Open Subtitles DataBase hash
   uint64_t osdbhash;
   //! Is loaded?
   bool loaded;
   //! File size
   uint64_t size;
   //! Where can we find a file
   vector<Path> paths;
   //! When was file added into database
   time_t added;
   //! Last time changed
   time_t checked;

protected:
   //! Update hash and filesize
   void update_info(const char* file);

   //! Constructor used when loading from database
   File(const string& md5hash, uint64_t osdbhash, uint64_t size,
        const vector<Path>& paths, time_t added = 0, time_t checked = 0):
                             md5hash(md5hash),
                             osdbhash(osdbhash),
                             loaded(true),
                             size(size),
                             paths(paths),
                             added(added),
                             checked(checked) {}
public:
   //! Constructor from file
   File(const char* file, const string& storage);
   //! Copy constructor
   File(const File& other):  db_id(other.db_id),
                             md5hash(other.md5hash),
                             osdbhash(other.osdbhash),
                             loaded(other.loaded),
                             size(other.size),
                             paths(other.paths),
                             added(other.added),
                             checked(other.checked) {}
   //! Move constructor
   File(const File&& other): db_id(other.db_id),
                             md5hash(other.md5hash),
                             osdbhash(move(other.osdbhash)),
                             loaded(other.loaded),
                             size(move(other.size)),
                             paths(move(other.paths)),
                             added(other.added),
                             checked(other.checked) {}

   void touch();
   void save();
   const vector<Path>& get_paths() const { return paths; }
   const string& get_md5_hash() const { return md5hash; };
   uint64_t get_osdb_hash() const { return osdbhash; };
   long get_size() const { return size; };
   bool is_loaded() const { return loaded; };

   //! Check whether files look same
   bool looks_same(const File& other) const {
      return ((md5hash == other.md5hash) && (osdbhash == other.osdbhash) &&
              (size == other.size));
   }

   //! Move all data from other file here
   void assimilate(File& other);
   //! Add new path to the file
   void add_path(Path& path);
   //! Remove path from the file
   void remove_path(Path& path);
};

#endif // STRUCTURES_HPP
