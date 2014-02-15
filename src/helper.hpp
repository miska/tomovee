#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <functional>

using namespace std;

//! Finds files in directory and executes action on them
void find(
      //! Path where to start looking
      char* path,
      //! What to do with files
      function<void(const string)> action       = [](const string arg) { },
      //! Check whether action should taken on this particular file
      function<bool(const string)> file_test    = [](const string arg) -> bool {
                                                          return true;
                                                     },
      //! Check whether descend into specified directory
      function<bool(const string)> recurse_test = [](const string arg) -> bool {
                                                          return true;
                                                     });

#endif // HELPER_HPP
