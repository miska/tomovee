/**
 * \brief Implementation of helper functions
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <magic.h>
#include <vector>
#include <string>
#include <fstream>

#include <tntdb/connection.h>
#include <tntdb/connect.h>
#include <tntdb/row.h>

#include "helper.hpp"
#include "sql_init.h"

std::string db_url;

//! Function to compute Open Subtitles DB hash
void compute_hash(uint64_t& osdb_hash, uint32_t& m_hash, FILE * handle) {
        uint64_t fsize;
        uint64_t tmp;
        size_t i;

        fseek(handle, 0, SEEK_END);
        fsize = ftell(handle);
        fseek(handle, 0, SEEK_SET);

        osdb_hash = fsize;
        m_hash = fsize;

        for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && 
            fread((char*)&tmp, sizeof(tmp), 1, handle);
            osdb_hash += tmp, i++);

        fseek(handle, (uint64_t)(
                        ((fsize/((uint64_t)2)) > (uint64_t)32768)?
                         (fsize/((uint64_t)2) - 32768) : 0), SEEK_SET);
        for(uint32_t tmp = 0, i = 0; i < (size_t)65536/sizeof(tmp) &&
            fread((char*)&tmp, sizeof(tmp), 1, handle);
            m_hash += tmp, i++);

        fseek(handle, (uint64_t)((fsize > (uint64_t)65536) ?
                                  fsize - 65536 : 0), SEEK_SET);
        for(uint64_t tmp = 0, i = 0; i < 65536/sizeof(tmp) &&
            fread((char*)&tmp, sizeof(tmp), 1, handle);
            osdb_hash += tmp, i++);
}

void init_db() {
   tntdb::Connection conn = tntdb::connectCached(db_url);
   conn.execute(SQL_INIT);
}

bool is_video(const char *file) {
   const char *magic_full;
   static bool init = true;
   static magic_t magic_cookie;

   if(init) {
      magic_cookie = magic_open(MAGIC_MIME);
      if(magic_cookie == NULL)
         throw "Can't open magic library";
      if(magic_load(magic_cookie, NULL) != 0)
         throw "Can't open magic file";
      init = false;
   }

   magic_full = magic_file(magic_cookie, file);
   return strncmp("video", magic_full, 5) == 0;
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

// TODO: optimise
std::string imdb_from_nfo(const char* cfile) {
   std::string ret;
   char *file = strdup(cfile);
   const char *patt = "://www.imdb.com/title/";
   size_t patt_len = strlen(patt);
   char* dir = file;

   while((strcmp(dir = dirname(dir), ".") != 0) && ret.empty()) {
      find(dir,
         [&](const char* name) {
            std::string str;
            std::ifstream file(name,std::ios::in);
            if (file) {
               while (!file.eof()) str.push_back(file.get());
            }
            auto f = str.find(patt);
            if(f != std::string::npos) {
               for(auto i = f + patt_len; i < str.size() && 
                                            ( str[i] == 't' || isdigit(str[i]));
                   ret.push_back(str[i]), i++);
            }
         },
         [](const char *name) -> bool { 
            int len = strlen(name);
            return ( (len > 4) &&
                     (tolower(name[len - 4]) == '.') &&
                     (tolower(name[len - 3]) == 'n') &&
                     (tolower(name[len - 2]) == 'f') &&
                     (tolower(name[len - 1]) == 'o') );
         },
         [&](const char *name) -> bool { return strcmp(name, dir) == 0; }
      );
   }
   free(file);
   return ret;
}
