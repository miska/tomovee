/**
 * \brief Implementation of helper functions
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
#include "structures.hpp"
#include "path.hpp"
#include "movie.hpp"
#include "file.hpp"

std::string db_url;

//! Function to compute Open Subtitles DB hash
void compute_hash(uint64_t& osdb_hash, uint32_t& m_hash, FILE * handle) {
        uint64_t fsize = 0;
        uint64_t tmp = 0;
        size_t i = 0;

        if(fseek(handle, 0, SEEK_END) < 0)
            return;
        if((fsize = ftell(handle)) == (uint64_t)-1)
            return;
        if(fseek(handle, 0, SEEK_SET) < 0)
            return;

        osdb_hash = fsize;
        m_hash = fsize;

        for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && 
            fread((char*)&tmp, sizeof(tmp), 1, handle);
            osdb_hash += tmp, i++);

        if(fseek(handle, (uint64_t)(
                        ((fsize/((uint64_t)2)) > (uint64_t)32768)?
                         (fsize/((uint64_t)2) - 32768) : 0), SEEK_SET) < 0)
            return;

        for(uint32_t tmp = 0, i = 0; i < (size_t)65536/sizeof(tmp) &&
            fread((char*)&tmp, sizeof(tmp), 1, handle);
            m_hash += tmp, i++);

        if(fseek(handle, (uint64_t)((fsize > (uint64_t)65536) ?
                                  fsize - 65536 : 0), SEEK_SET) < 0)
            return;

        for(uint64_t tmp = 0, i = 0; i < 65536/sizeof(tmp) &&
            fread((char*)&tmp, sizeof(tmp), 1, handle);
            osdb_hash += tmp, i++);
}

void init_db() {
   File::db_init();
   Path::db_init();
   Movie::db_init();
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
            if(stat(buff.c_str(), &st) != 0)
               continue;
            // Is it a directory?
            if(S_ISDIR(st.st_mode)) {
               // Put it at the front of the list
               // Doing depth first to limit memory usage
               recurse.push_front(buff);
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

void meta_from_nfo(const char* cfile, File& f) {
   const char *patt = "://www.imdb.com/title/";
   size_t patt_len = strlen(patt);
   char* dir2 = strdup(cfile);
   char* dir2_d = strdup(dir2);
   char* dir1 = strdup(dirname(dir2_d));
   char* dir1_d = strdup(dir1);

   // Descend lower and lower till we find what we need or run out of options
   while(strcmp(dir1, dir2) != 0) {
      find(dir1,
         [&](const char* name) {
            bool force = name[strlen(name)-1] == 'e';
            if((f.get_movie_assigned_by() != NOT_ASSIGNED &&
               !f.get_subtitles().empty() &&
               !f.get_audios().empty()) && !force)
                return;

            // Read whole file into memory
            std::string str;
            std::ifstream file(name,std::ios::in);
            if(file) {
               while(!file.eof()) str.push_back(tolower(file.get()));
            }

            // Try to get IMDB id
            size_t s = str.find(patt);
            std::string imdb;
            if(s != std::string::npos) {
               for(size_t i = s + patt_len; i < str.size() &&
                                        ( str[i] == 't' || isdigit(str[i]));
                   imdb.push_back(str[i]), i++);
            }

            if(!imdb.empty() && (f.get_movie_assigned_by() == NOT_ASSIGNED ||
                                 force)) {
               std::vector<Movie> m = Movie::search("imdb_id = :imdb",
                  [&imdb](tntdb::Statement& st) { st.setString("imdb", imdb); }
               );
               if(m.empty()) {
                  m.push_back(Movie(imdb));
               }
               f.set_movie_assigned_by(AUTO_NFO);
               m.front().add_file(f);
            }

            // Try to find language information
            s = str.find("audio");
            // Try to find subtitle information
            s = str.find("subtitle");
         },
         // We want only nfo files
         [](const char *name) -> bool { 
            int len = strlen(name);
            std::string nme;
            std::string pat1 = ".nfo";
            std::string pat2 = ".nfo.force";

            for(auto i = 0; i<len; i++)
               nme.push_back(tolower(name[i]));

            std::string::reverse_iterator p1 = pat1.rbegin();
            std::string::reverse_iterator p2 = pat2.rbegin();
            for(auto it = nme.rbegin(); it != nme.rend(); it++) {
               if((*p1 != *it) && (*p2 != *it))
                  return false;

               if(*it != *p1)
                  p1 = p2;
               else
                  p1++;

               if(*it != *p2)
                  p2 = p1;
               else
                  p2++;

               if((p1 == pat1.rend()) || (p2 == pat2.rend()))
                  return true;
            }
            return false;
         },
         // Descend just to current directory
         [&](const char *name) -> bool { return strcmp(name, dir1) == 0; }
      );
      free(dir2);
      free(dir2_d);
      dir2 = dir1;
      dir2_d = dir1_d;
      dir1 = strdup(dirname(dir2_d));
      dir1_d = strdup(dir1);
   }
   free(dir2);
   free(dir2_d);
   free(dir1);
   free(dir1_d);
}

void get_movie_info(const char* file, int32_t& length, int32_t& width, int32_t& height,
                    std::string& audio, std::string& srt) {
   int p[2];
   pid_t f;
   static char buff[2048], *sk, *ptr;
   ssize_t r;

   length = height = width = -1;
   audio = srt = "";

   errno = 0;
   if(pipe(p) != 0) {
      fprintf(stderr, "Can't create pipe!\n");
      fprintf(stderr, "ERROR: %s!\n", strerror(errno));
      return;
   }

   // Run avprobe or ffprobe
   errno = 0;
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
      close(p[1]);
      exit(0);
   }

   if(f < 0) {
      fprintf(stderr, "Can't fork!\n");
      fprintf(stderr, "ERROR: %s!\n", strerror(errno));
      return;
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
         sk++;
         while(*sk == ' ') sk++;
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
   close(p[0]);
   wait(NULL);
}
