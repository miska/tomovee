/**
 * \brief Header for helper functions
 */

#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <functional>
#include <limits.h>
#include <file.hpp>

extern std::string db_url;

using namespace std;

//! Function to compute Open Subtitles DB hash
void compute_hash(uint64_t& osdb, uint32_t& m, FILE * handle);

//! Finds files in directory and executes action on them
void find(
      //! Path where to start looking
      const char* path,
      //! What to do with files
      function<void (const char*)> action       = [](const char*) { },
      //! Check whether action should taken on this particular file
      function<bool (const char*)> file_test    = [](const char*) -> bool {
                                                          return true;
                                                     },
      //! Check whether descend into specified directory
      function<bool (const char*)> recurse_test = [](const char*) -> bool {
                                                          return true;
                                                     });

//! Checks whether name contains the right extension
bool is_interesting(string name);

//! Checks whether file contains video
bool is_video(const char *file);

//! Makes sure database tables are created
void init_db();

void meta_from_nfo(const char* file, File& f);

void get_movie_info(const char* file, int32_t& length, int32_t& width, int32_t& height,
                    std::string& audio, std::string& srt);

#endif // HELPER_HPP
