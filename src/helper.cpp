/**
 * \brief Implementation of helper functions
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include "helper.hpp"

//! Finds files in directory and executes action on them
void find(const char* path,
          function<void(const char*)> action,
          function<bool(const char*)> file_test,
          function<bool(const char*)> recurse_test)
{

   // Check we want to be here
   if(!recurse_test(path))
      return;

   // Open directory
   DIR * dir = opendir(path);

   if(dir == NULL) return;

   struct stat st;
   string l_path(path);
   static char buff[PATH_MAX];
   dirent* entry = NULL;

   // Iterate through directory content
   while((entry = readdir(dir)) != NULL) {
      if((strcmp(entry->d_name,".") != 0) && (strcmp(entry->d_name,"..") != 0)) {
         snprintf(buff,PATH_MAX,"%s/%s",l_path.c_str(),entry->d_name);
         stat(buff, &st);
         if(S_ISDIR(st.st_mode)) {
            find(buff, action, file_test, recurse_test);
         } else if(S_ISREG(st.st_mode)) {
            if(file_test(buff)) {
               action(buff);
            }
         }
      }
   }
   closedir(dir);
}
