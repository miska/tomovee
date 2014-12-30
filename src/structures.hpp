/**
 * \brief Definitions of objects used in the application
 */

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <vector>
#include <string>
#include <time.h>

using namespace std;

enum movie_assigned {
   NOT_ASSIGNED = 0,
   MANUAL,
   AUTO_NFO,
};

class File;

//! Basic class to represent a path
class Path {
friend class File;
private:
   //! ID in the database
   uint64_t db_id;
   //! On which storage is this path
   string storage;
   //! Path relative to the root of the storage
   string path;
   //! Last time changed
   time_t checked;
protected:
   //! Constructor
   Path(const string& st, const string& pth, uint64_t parent);
public:
   //! Copy constructor
   Path(const Path& other):  db_id(other.db_id),
                             storage(other.storage),
                             path(other.path),
                             checked(other.checked) {}

   //! Move constructor
   Path(const Path&& other): db_id(other.db_id),
                             storage(move(other.storage)),
                             path(move(other.path)),
                             checked(other.checked) {}
   //! Assignment operator
   Path& operator=(const Path& other);
   //! Comparison operator
   bool operator==(const Path& other) const;
   //! Comparison operator
   bool operator!=(const Path& other) { return (!((*this) == other)); }
   //! Delete path from database
   void remove();
   //! Update last checked time
   void touch();
   //! Clean all files not checked since
   static void cleanup(time_t older, std::string storage);
};

//! Basic class to represent a file
class File {
   //! ID in the database
   uint64_t db_id;
   //! Just to be sure, checksum from the middle of the file
   uint32_t mhash;
   //! File Open Subtitles DataBase hash
   uint64_t osdbhash;
   //! File size
   uint64_t size;
   //! Where can we find a file
   vector<Path> paths;
   //! When was file added into database
   time_t added;
   std::string audios;
   std::string subtitles;
   int width, height, length;

protected:

   //! Constructor used when loading from database
   File(uint32_t id, uint32_t mhash, uint64_t osdbhash, uint64_t size,
        const vector<Path>& paths, time_t added = 0):
                             db_id(id),
                             mhash(mhash),
                             osdbhash(osdbhash),
                             size(size),
                             paths(paths),
                             added(added) {}
public:
   //! Update hash and filesize
   void update_info(const char* file);
   //! Update movie meta data like resolution, streams and length
   void update_meta(const char* file);
   //! Constructor from file
   File(const char* file, const string& storage);
   //! Copy constructor
   File(const File& other):  db_id(other.db_id),
                             mhash(other.mhash),
                             osdbhash(other.osdbhash),
                             size(other.size),
                             paths(other.paths),
                             added(other.added),
                             audios(other.audios),
                             subtitles(other.subtitles),
                             width(other.width),
                             height(other.height),
                             length(other.length) {}
   //! Move constructor
   File(const File&& other): db_id(other.db_id),
                             mhash(other.mhash),
                             osdbhash(move(other.osdbhash)),
                             size(move(other.size)),
                             paths(move(other.paths)),
                             added(other.added),
                             audios(move(other.audios)),
                             subtitles(move(other.subtitles)),
                             width(other.width),
                             height(other.height),
                             length(other.length) {}

   const vector<Path>& get_paths() const { return paths; }

   //! Check whether files look same
   bool looks_same(const File& other) const {
      return ((mhash == other.mhash) && (osdbhash == other.osdbhash) &&
              (size == other.size));
   }

   //! Return hash for Open Subtitle DataBase
   uint64_t get_osdb_hash() { return osdbhash; }
   //! Return size of the file
   uint64_t get_size() { return size; }
   //! Set movie this file belongs to
   void set_movie(movie_assigned type, uint64_t movie_id);
   //! Clean files that we do not have in any storage
   static void cleanup();
};

class Movie {
   uint64_t db_id;
public:
   std::string imdb_id;
   Movie(std::string imdb_id);
   uint64_t get_id() { return db_id; }
};

#endif // STRUCTURES_HPP
