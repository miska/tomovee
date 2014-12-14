/**
 * \brief Implementation of helper functions
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <string>

#include <tntdb/connection.h>
#include <tntdb/connect.h>
#include <tntdb/row.h>

#include "helper.hpp"
#include "sql_init.h"

std::string db_url;

void init_db() {
   tntdb::Connection conn = tntdb::connectCached(db_url);
   conn.execute(SQL_INIT);
}

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

/*
 * TODO: Implement properly
 *       * No fixed list of extensions
 *       * Aho-Corasic
 */
//! Checks whether name contains the right extension
bool is_interesting(string name) {
   vector<string> extensions = { ".avi", ".mkv", ".mov", ".mp4" };
   
   for(auto ext : extensions) {
      auto i = name.rbegin();
      auto j = ext.rbegin();
      while(i != name.rend() && j != ext.rend() && tolower(*i) == tolower(*j))
          i++,j++;
      if(j==ext.rend())
         return true;
   }
   return false;
}

