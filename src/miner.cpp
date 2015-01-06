/**
 * \brief Application to parse filesystem and gather data
 */

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "helper.hpp"
#include "structures.hpp"

using namespace std;

bool should_scan(const char* where) {
   std::string tmp = where;
   tmp += "/.no_video";
   return access(tmp.c_str(), F_OK) != 0;
}

void print_help(const char* argv_0) {
   char *dp = strdup(argv_0);
   printf("Usage info:\n");
   printf("  %s [option] <path>\n", basename(dp));
   printf("\n");
   printf("Options:\n");
   printf(" -s, --storage <st>  This storage is called <st>.\n");
   printf(" -d, --delete        Delete from database paths no longer found in this run.\n");
   printf(" -D, --delete-all    Delete from database paths no longer found in this run\n");
   printf("                     and files without any path.\n");
   printf(" -v, --verbose       Be more verbose.\n");
   printf(" -h, --help          Show this help.\n");
   printf("\n");
   free(dp);
}

//! Main for miner application
int main(int argc, char **argv) {
   static struct option opts[] = {
      {"storage",     required_argument, NULL, 's' },
      {"delete",      no_argument      , NULL, 'd' },
      {"delete-all",  no_argument      , NULL, 'D' },
      {"verbose",     no_argument      , NULL, 'v' },
      {"help"   ,     no_argument      , NULL, 'h' },
      { 0       ,     0                , 0   ,  0  }
   };
   int c;
   bool verbose = false;
   bool del = false;
   bool del_all = false;
   time_t del_ts = time(NULL);
   int index;
   const char *outdir = NULL;
   char buff[128];
   string path, storage;

   // Parse the options
   while((c = getopt_long(argc, argv, "hdDvs:", opts, &index)) != -1) {
      switch(c) {
      case 'h':
         print_help(argv[0]);
         exit(0);
      case 's':
         storage = optarg;
         break;
      case 'v':
         verbose = true;
         break;
      case 'd':
         del = true;
         break;
      case 'D':
         del = true;
         del_all = true;
         break;
      default:
      case '?':
         print_help(argv[0]);
         exit(-1);
      }
   }

   if(optind != argc-1) {
      print_help(argv[0]);
      exit(-1);
   }

   if(storage.empty()) {
      gethostname(buff, 127);
      storage = buff;
   }

   // Where to store data?
   outdir = getenv("XDG_CONFIG");
   if(outdir == NULL)
      outdir = getenv("HOME");
   if(outdir == NULL)
      outdir = ".";

   db_url = std::string("sqlite:") + outdir + "/.tomovee.sqlite";
   init_db();

   printf("Scanning directory '%s' in storage '%s'\n", argv[argc-1], storage.c_str());
   if(chdir(argv[argc-1]) != 0) {
      fprintf(stderr, "Can't chdir into %s!\n", argv[argc-1]);
      exit(-1);
   }
   find(".",
        [&](const char* name) {
           auto f = File(name + 2, storage);
           if(verbose) {
              if(time(NULL) - f.get_added() > 1) {
                 printf("File '%s' was already into database.\n", name + 2);
              } else {
                 printf("File '%s' added into database.\n", name + 2);
              }
           }
           std::string i = imdb_from_nfo(name);
           if(verbose && !i.empty()) {
              printf("Got IMDB ID '%s' from NFO file.\n", i.c_str());
              auto m = Movie(i);
              f.set_movie(AUTO_NFO, m.get_id());
           }
        },
        [](const char* name) -> bool { return is_video(name); },
        verbose?
           [](const char* name) -> bool { if(should_scan(name)) {
                                             printf("Entering directory %s\n", name);
                                             return true;
                                          } else {
                                             printf("Skipping directory %s\n", name);
                                             return false;
                                          }
                                        }
        :
           [](const char* name) -> bool { return should_scan(name); }
       );
   if(del) {
      Path::cleanup(del_ts, storage);
      if(del_all)
         File::cleanup();
   }
}
