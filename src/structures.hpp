#ifndef MOVIE_HPP
#define MOVIE_HPP

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
}

//! Basic class to represent a file
class File {
protected:
   //! File hash
   string hash;
   //! File size
   long size;
   //! Update hash and filesize
   void update_info();
public:
   //! Constructor from file
   File(const char* file);
   //! Copy constructor
   File(const File& other):  paths(other.paths),
                             hash(other.hash),
                             size(other.size) {}
   //! Move constructor
   File(const File&& other): paths(move(other.paths)),
                             hash(move(other.hash)),
                             size(move(other.size)) {}
   //! Where can we find a file
   vector<Path> paths;
   //! Returns files hash
   string get_hash();
   //! Returns files size
   long get_size();
}

#endif // MOVIE_HPP
