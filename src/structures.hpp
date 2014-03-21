/**
 * \brief Definitions of objects used in the application
 */

#ifndef STRUCTURES_HPP
#define STRUCTURE_HPP

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
   bool operator==(const Path& other);
   //! Comparison operator
   bool operator!=(const Path& other) { return (!((*this) == other)); }
};

//! Basic class to represent a file
class File {
protected:
   //! File hash
   string hash;
   //! Is loaded?
   bool loaded;
   //! File size
   long size;
   //! Update hash and filesize
   void update_info(const char* file);
public:
   //! Constructor from file
   File(const char* file, const string& storage);
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
   //! Where can we find a file
   vector<Path> paths;
   //! Returns files hash
   string get_hash() { return hash; };
   //! Returns files size
   long get_size() { return size; };
   //! Returns whether data about file are loaded
   bool is_loaded() { return loaded; };
   //! Check whether files look same
   bool looks_same(const File& other) {
      return ((hash == other.hash) && (size == other.size));
   }
   //! Move all data from other file here
   void assimilate(File& other);
};

#endif // STRUCTURES_HPP
