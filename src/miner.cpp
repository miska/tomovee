/**
 * \brief Application to parse filesystem and gather data
 */

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string>
#include <set>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "helper.hpp"
#include "structures.hpp"

using namespace std;

std::set<std::string> exclude;

bool should_scan(const char* where) {
   std::string tmp = where;
   tmp += "/.no_video";
   return (access(tmp.c_str(), F_OK) != 0) && 
          (exclude.find(where) == exclude.end());
}

void print_help(const char* argv_0) {
   char *dp = strdup(argv_0);
   printf("Usage info:\n");
   printf("  %s [option] <path>\n", basename(dp));
   printf("\n");
   printf("Options:\n");
   printf(" -s, --storage <st>  This storage is called <st>.\n");
   printf(" -m, --mtime         Use modification time to indicate when was the file added.\n");
   printf(" -d, --delete        Delete from database paths no longer found in this run.\n");
   printf(" -D, --delete-all    Delete from database paths no longer found in this run\n");
   printf("                     and files without any path.\n");
   printf(" -i, --include       Include only following directories (path relative to the root)\n");
   printf(" -e, --exclude       Exclude following directories (path relative to the root)\n");
   printf(" -v, --verbose       Be more verbose.\n");
   printf(" -h, --help          Show this help.\n");
   printf("\n");
   free(dp);
}

//! Main for miner application
int main(int argc, char **argv) {
   static struct option opts[] = {
      {"storage",     required_argument, NULL, 's' },
      {"exclude",     required_argument, NULL, 'e' },
      {"include",     required_argument, NULL, 'i' },
      {"delete",      no_argument      , NULL, 'd' },
      {"mtime",       no_argument      , NULL, 'm' },
      {"delete-all",  no_argument      , NULL, 'D' },
      {"verbose",     no_argument      , NULL, 'v' },
      {"help"   ,     no_argument      , NULL, 'h' },
      { 0       ,     0                , 0   ,  0  }
   };
   int c;
   bool verbose = false;
   bool del = false;
   bool mtime = false;
   bool del_all = false;
   std::set<std::string> include;
   time_t del_ts = time(NULL);
   int index;
   const char *outdir = NULL;
   char buff[128];
   string path, storage;

   // Parse the options
   while((c = getopt_long(argc, argv, "mhdDvs:e:i:", opts, &index)) != -1) {
      switch(c) {
      case 'h':
         print_help(argv[0]);
         exit(0);
      case 's':
         storage = optarg;
         break;
      case 'm':
         mtime = true;
         break;
      case 'e':
         if(optarg[0] == '.')
            exclude.insert(optarg);
         else
            exclude.insert(std::string("./") + optarg);
         break;
      case 'i':
         if(optarg[0] == '.')
            include.insert(optarg);
         else
            include.insert(std::string("./") + optarg);
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
   if(getenv("TOMOVEE_DB") == NULL) {
      outdir = getenv("XDG_CONFIG");
      if(outdir == NULL)
         outdir = getenv("HOME");
      if(outdir == NULL)
         outdir = ".";

      db_url = std::string("sqlite:") + outdir + "/.tomovee.sqlite";
   } else {
      db_url = getenv("TOMOVEE_DB");
   }

   init_db();

   printf("Scanning directory '%s' in storage '%s'\n", argv[argc-1], storage.c_str());
   if(chdir(argv[argc-1]) != 0) {
      fprintf(stderr, "Can't chdir into %s!\n", argv[argc-1]);
      exit(-1);
   }
   if(include.empty())
      include.insert(".");
   for(auto i:include)
   find(i.c_str(),
        [&](const char* name) {
           auto f = create_file(storage.c_str(), name + 2, mtime);
           if(verbose) {
              if(!mtime) {
              if(time(NULL) - f.get_added() > 1) {
                 printf("Checked file '%s' as it was already in the database.\n", name + 2);
              } else {
                 printf("Added file '%s' into database.\n", name + 2);
              } } else {
                 static char buff[128];
                 time_t added = f.get_added();
                 strftime(buff, sizeof(buff), "%F %T", localtime(&added));
                 printf("File '%s' was added on %s.\n", name + 2, buff);
              }

           }
           std::string i = imdb_from_nfo(name);
           if(verbose && !i.empty()) {
              printf("Got IMDB ID '%s' from NFO file.\n", i.c_str());
              auto m = Movie(i);
              m.save();
              f.set_movie_assigned_by(AUTO_NFO);
              f.set_parent_movie(m);
              f.save();
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
/*      Path::remove("checked < :ts AND storage = :st", del_ts, storage);
      if(del_all)
         File::cleanup(); */
   }
}
