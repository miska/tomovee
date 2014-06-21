/**
 * \brief Definitions of objects used in the application
 */

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <vector>
#include <string>

using namespace std;

//! Basic class to represent a path
class Path {
public:
   //! On which storage is this path
   string storage;
   //! Path relative to the root of the storage
   string path;
   //! Constructor
   Path(const string& st, const string& pth):
                             storage(st),
                             path(pth) {}
   //! Copy constructor
   Path(const Path& other):  storage(other.storage),
                             path(other.path) {}
   //! Move constructor
   Path(const Path&& other): storage(move(other.storage)),
                             path(move(other.path)) {}
   //! Assignment operator
   Path& operator=(const Path& other);
   //! Comparison operator
   bool operator==(const Path& other) const;
   //! Comparison operator
   bool operator!=(const Path& other) { return (!((*this) == other)); }
};

//! Basic class to represent a file
class File {

   //! File hash
   string hash;
   //! Is loaded?
   bool loaded;
   //! File size
   long size;
   //! Where can we find a file
   vector<Path> paths;
   
   //! Update hash and filesize
   void update_info(const char* file);
public:
   //! Constructor from file
   File(const char* file, const string& storage);
   //! Constructor from all data - database
   File(const string& hash, long size, const vector<Path>& paths):
                             hash(hash),
                             loaded(true),
                             size(size),
                             paths(paths) {}
   //! Copy constructor
   File(const File& other):  hash(other.hash),
                             loaded(other.loaded),
                             size(other.size),
                             paths(other.paths) {}
   //! Move constructor
   File(const File&& other): hash(move(other.hash)),
                             loaded(other.loaded),
                             size(move(other.size)),
                             paths(move(other.paths)) {}


   const vector<Path>& get_paths() const { return paths; }
   const string& get_hash() const { return hash; };
   long get_size() const { return size; };
   bool is_loaded() const { return loaded; };

   //! Check whether files look same
   bool looks_same(const File& other) const {
      return ((hash == other.hash) && (size == other.size));
   }

   //! Move all data from other file here
   void assimilate(File& other);
};

#endif // STRUCTURES_HPP
