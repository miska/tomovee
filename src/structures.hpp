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
};

//! Basic class to represent a file
class File {
protected:
   //! File hash
   string hash;
   //! File size
   long size;
   //! Update hash and filesize
   void update_info(const char* file);
public:
   //! Constructor from file
   File(const char* file, const string& storage);
   //! Copy constructor
   File(const File& other):  hash(other.hash),
                             size(other.size),
                             paths(other.paths) {}
   //! Move constructor
   File(const File&& other): hash(move(other.hash)),
                             size(move(other.size)),
                             paths(move(other.paths)) {}
   //! Where can we find a file
   vector<Path> paths;
   //! Returns files hash
   const string& get_hash() { return hash; };
   //! Returns files size
   long get_size() { return size; };
};

#endif // STRUCTURES_HPP
