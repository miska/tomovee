/**
 * \brief Application to interact with the database
 */

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cxxtools/jsonserializer.h>

#include "helper.hpp"
#include "structures.hpp"

using namespace std;

bool first = true;

void display_file(File &f) {
   static char buff[1024];
   time_t tmp = f.get_added();
   strftime(buff, sizeof(buff), "%F %T", localtime(&tmp));
   if(!first)
      printf("\n");
   printf("File:\n");
   printf("  added:      %s\n", buff);
   if(f.get_width() > 0 && f.get_height() > 0)
      printf("  resolution: %dx%d\n", f.get_width(), f.get_height());
   if(f.get_length() > 0)
      printf("  length:     %d:%02d:%02d\n", f.get_length()/3600, (f.get_length()%3600)/60, f.get_length()%60);
   printf("  paths:\n");
   for(auto pth : f.get_paths())
      printf("    %s://%s\n", pth.get_storage().c_str(), pth.get_path().c_str());
   first = false;
}

void print_help(const char* argv_0) {
   char *dp = strdup(argv_0);
   printf("Usage info:\n");
   printf("  %s [options] <command> [arguments]\n", basename(dp));
   printf("\n");
   printf("Options:\n");
   printf("   -c <count>        Show <count> files at maximum\n");
   printf("   -s <storage>      Restrict queries to <storage>\n");
   printf("\n");
   printf("Commands:\n");
   printf("   new               Show of new files/movies\n");
   printf("   search <pattern>  Search for <pattern> in the cached filenames\n");
   printf("   help              Show this help\n");
   printf("\n");
   free(dp);
}

//! Main for miner application
int main(int argc, char **argv) {
   string storage;
   int limit=10;
   const char* outdir;

   int optind = 1;

   if(optind >= argc) {
      print_help(argv[0]);
      exit(-1);
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

   int last_optind;
   while(optind < argc) {
      last_optind = optind;
      if(optind < argc && (strcmp(argv[optind],"-s") == 0)) {
         optind++;
         if(optind >= argc) {
            print_help(argv[0]);
            exit(1);
         }
         storage = argv[optind];
         optind++;
      }
      if(optind < argc && (strcmp(argv[optind],"-c") == 0)) {
         optind++;
         if(optind >= argc) {
            print_help(argv[0]);
            exit(1);
         }
         if(sscanf(argv[optind], "%d", &limit) == 1) {
            optind++;
         } else {
            print_help(argv[0]);
            exit(1);
         }
      }
      if(optind < argc && strcmp(argv[optind],"new")==0) {
         optind++;
         for(auto f : File::latest(limit, storage))
            display_file(f);
      }
      if(optind < argc && strcmp(argv[optind],"export")==0) {
         optind++;
         cxxtools::SerializationInfo si;
         File::for_all([&si](File f) {
                           cxxtools::SerializationInfo& nsi = si.addMember("");
                           nsi <<= f;
                       }, storage);
         si.setTypeName("set");
         si.setCategory(cxxtools::SerializationInfo::Array);
         cxxtools::JsonSerializer json(std::cout);
         json.beautify(true);   // this makes it just nice to read
         json.serialize(si);
         json.finish();
      }
      if(optind < argc && strcmp(argv[optind],"search")==0) {
         optind++;
         if(optind >= argc) {
            print_help(argv[0]);
            exit(1);
         }
         for(auto f : File::search(argv[optind], limit, storage))
            display_file(f);
         optind++;
      }
      if(optind < argc && strcmp(argv[optind],"help")==0) {
         print_help(argv[0]);
         exit(0);
      }
      if(last_optind == optind) {
         print_help(argv[0]);
         exit(1);
      }
   }
}
