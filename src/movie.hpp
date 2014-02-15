#ifndef MOVIE_HPP
#define MOVIE_HPP

#include <vector>
#include <string>

using namespace std;

//! Basic class to represent movie in collection
class movie {
   protected:
      //! All existing paths to the file
      vector<string> paths;
      //! Murmur hash of file
      string hash;
   public:
      //! Created from file
      movie(string path);
      //! Copy constructor
      movie(const movie& other):
            paths((other.paths)),
            hash((other.hash)) {}
      //! Move constructor
      movie(const movie&& other):
            paths(move(other.paths)),
            hash(move(other.hash)) {}
      //! Get Murmur hash of file
      string get_hash();
      //! Merge with other movie entry
      void assimilate(movie& other);
}

#endif // MOVIE_HPP
