/**
 * \brief Application to interact with the database
 */

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "helper.hpp"
#include "structures.hpp"

using namespace std;

void print_help(const char* argv_0) {
   char *dp = strdup(argv_0);
   printf("Usage info:\n");
   printf("  %s [-s <storage>] <command> [options]\n", basename(dp));
   printf("\n");
   printf("Commands:\n");
   printf("   new [number]  Show [number] (default 100) of new files/movies\n");
   printf("   help          Show this help\n");
   printf("\n");
   free(dp);
}

//! Main for miner application
int main(int argc, char **argv) {
   string storage;
   const char* outdir;

   int optind = 1;
   if((argc > optind) && (strcmp(argv[optind],"-s") == 0)) {
      storage = argv[optind+1];
      optind += 2;
   }

   if(optind >= argc) {
      print_help(argv[0]);
      exit(-1);
   }

   if(strcmp(argv[optind],"help")==0) {
      print_help(argv[0]);
      exit(0);
   }

   // Where to store data?
   outdir = getenv("XDG_CONFIG");
   if(outdir == NULL)
      outdir = getenv("HOME");
   if(outdir == NULL)
      outdir = ".";

   db_url = std::string("sqlite:") + outdir + "/.tomovee.sqlite";
   init_db();

   if(strcmp(argv[optind],"new")==0) {
      int limit=100;
      if(optind+1 < argc)
         sscanf(argv[optind+1], "%d", &limit);
      char buff[1024];
      for(auto f : File::latest(limit)) {
         time_t tmp = f.get_added();
         strftime(buff, sizeof(buff), "%F %T", localtime(&tmp));
         printf("%s : \n", buff);
         for(auto pth : f.get_paths())
            printf("   %s : %s\n", pth.get_storage().c_str(), pth.get_path().c_str());
      }
   }
}
