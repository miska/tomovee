/**
 * \brief Implementation of helper functions
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <locale.h>
#include <magic.h>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <errno.h>

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
          function<bool(const char*)> recurse_test) {

   std::list<string> recurse;
   recurse.push_back(path);

   while(!recurse.empty()) {
      // Get top of the queue
      string pth = recurse.front();
      recurse.pop_front();

      // Check we want to be here
      if(!recurse_test(pth.c_str())) {
         continue;
      }

      // Open directory
      errno = 0;
      DIR * dir = opendir(pth.c_str());
      if(dir == NULL) {
         fprintf(stderr, "Error opening directory '%s'!\n%s", path, strerror(errno));
         return;
      }

      // Iterate through directory content
      dirent* entry = NULL;
      string buff;
      struct stat st;
      while((entry = readdir(dir)) != NULL) {
         if((strcmp(entry->d_name,".") != 0) && (strcmp(entry->d_name,"..") != 0)) {
            buff = pth + "/" + entry->d_name;
            stat(buff.c_str(), &st);
            // Is it a directory?
            if(S_ISDIR(st.st_mode)) {
               // Put it at the end of the list
               recurse.push_back(buff);
            // Is it regular file?
            } else if(S_ISREG(st.st_mode)) {
               if(file_test(buff.c_str()))
                  action(buff.c_str());
            }
         }
      }
      closedir(dir);
   }
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

void get_movie_info(const char* file, int& length, int& width, int& height,
                    std::string& audio, std::string& srt) {
   int p[2];
   pid_t f;
   static char buff[2048], *sk, *ptr;
   ssize_t r;

   length = height = width = -1;
   audio = srt = "";

   if(pipe(p) != 0)
      return;

   // Run avprobe or ffprobe
   if((f = fork()) == 0) {
      close(p[0]);
      close(1);
      close(2);
      dup2(p[1], 1);
      dup2(p[1], 2);
      close(p[1]);
      setlocale(LC_ALL, "");
      execlp("avprobe", "avprobe", file, (char*)NULL);
      execlp("ffprobe", "ffprobe", file, (char*)NULL);
      exit(0);
   }

   // Read the output
   close(p[1]);
   r = 1;
   while(r > 0) {
      buff[0] = ' ';
      for(sk = buff; (sk < (buff + 2046)) && ((*(sk-1)) != '\n')
          && (r > 0); r = read(p[0], sk++, 1));
      (*(sk-1)) = 0;

      // Get duration of the movie
      if((sk = strstr(buff, "Duration: ")) != NULL) {
         sk = sk + 10;
         int h, m, s, ms;
         if(sscanf(sk, "%d:%d:%d.%d", &h, &m, &s, &ms) == 4) {
            length = h * 3600 + m * 60 + s + (ms>49?1:0);
         }
      }

      // Get movie resolution
      if((sk = strstr(buff, "Stream #")) != NULL &&
         (sk = strstr(++sk, "Video: ")) != NULL &&
         (sk = strstr(++sk,  ",")) != NULL &&
         (sk = strstr(++sk,  ",")) != NULL) {
         sk+=2;
         sscanf(sk,"%dx%d", &width, &height);
      }

      // Get audio track languages
      if((      sk = strstr(buff, "Stream #")) != NULL &&
         (ptr = sk = strstr(++sk, "Audio: ")) != NULL &&
         (      sk = strstr(buff, "(")) != NULL && sk++ < ptr &&
         strncmp(sk, "und)", 4) != 0) {
         if(!audio.empty())
            audio.push_back(',');
         while(isalpha(*sk)) {
            audio.push_back(*sk);
            sk++;
         }
      }

      // Get subtitles track languages
      if((      sk = strstr(buff, "Stream #")) != NULL &&
         (ptr = sk = strstr(++sk, "Subtitle: ")) != NULL &&
         (      sk = strstr(buff, "(")) != NULL && sk++ < ptr &&
         strncmp(sk, "und)", 4) != 0) {
         if(!srt.empty())
            srt.push_back(',');
         while(isalpha(*sk)) {
            srt.push_back(*sk);
            sk++;
         }
      }
   }
}
