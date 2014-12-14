/**
 * \brief Header for helper functions
 */

#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <functional>

extern std::string db_url;

using namespace std;

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

void init_db();

#endif // HELPER_HPP
