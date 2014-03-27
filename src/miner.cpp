/**
 * \brief Application to parse filesystem and gather data
 */

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <getopt.h>

#include "database.hpp"
#include "helper.hpp"
#include "structures.hpp"

using namespace std;

void print_help(const char* argv_0) {
   printf("%s help\n", argv_0);
}

//! Main for miner application
int main(int argc, char **argv) {
   static struct option opts[] = {
      {"storage", required_argument, NULL, 's' },
      {"verbose", no_argument      , NULL, 'v' },
      {"help"   , no_argument      , NULL, 'h' },
      { 0       , 0                , 0   ,  0  }
   };
   int c;
   bool verbose = false;
   int index;
   const char *outdir = NULL;
   char buff[128];
   string path, storage;

   // Parse the options
   while((c = getopt_long(argc, argv, "hvs:", opts, &index)) != -1) {
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

   open_sqlite_db(string(outdir) + "/." + "tomovee.sqlite");

   printf("Scanning directory '%s' for storage '%s'\n", argv[argc-1], storage.c_str());
   find(argv[argc-1],
        [&](const char* name) {
           DB->update_file(File(name, storage));
           if(verbose)
              printf("File '%s' added into database...\n", name);
        },
        [](const char* name) -> bool { return is_interesting(name); },
        verbose?
           [](const char* name) -> bool { printf(" ... %s\n", name); return true; }
        :
           [](const char*) -> bool { return true; }
       );
}
