/**
 * \brief Definitions of objects used in the application
 */

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <vector>
#include <string>
#include <time.h>
#include <functional>
#include <cxxtools/serializationinfo.h>

#include "path.hpp"
#include "file.hpp"
#include "movie.hpp"

using namespace std;

enum movie_assigned {
   NOT_ASSIGNED = 0,
   MANUAL,
   AUTO_NFO,
};

//! Update last checked time
void touch_path(Path& what, time_t when = 0);

//! Create File class from file
File create_file(const char* storage, const char* file, bool use_mtime = false);

//! Update hash and filesize
void update_file_info(File& target, const char* file, bool use_mtime = false);

//! Update movie meta data like resolution, streams and length
void update_file_meta(File& target, const char* file);

#endif // STRUCTURES_HPP
