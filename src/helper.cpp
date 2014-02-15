#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include "helper.hpp"

//! Finds files in directory and executes action on them
void find(const char* path,
      function<void(const char*)> action,
      function<bool(const char*)> file_test,
      function<bool(const char*)> recurse_test) {

   // Check we want to be here
   if(!recurse_test(path))
      return;

   // Open directory
   DIR * dir = opendir(path);
   char buff[PATH_MAX];
   dirent* entry = NULL;
   if(dir == NULL)
      return;

   // Iterate through directory content
   while((entry = readdir(dir)) != NULL) {
      if(((entry->d_type == DT_DIR) && (
          (strcmp(entry->d_name,".") != 0) && (strcmp(entry->d_name,"..") != 0)
         )) || (entry->d_type == DT_REG)) {
         snprintf(buff,PATH_MAX,"%s/%s",path,entry->d_name);
         if(entry->d_type == DT_DIR) {
            find(buff, action, file_test, recurse_test);
         }
         if(entry->d_type == DT_REG) {
            if(file_test(buff)) {
               action(buff);
            }
         }
      }
   }
end:
   closedir(dir);
}
