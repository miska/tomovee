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
#include <cxxtools/jsondeserializer.h>
#include <cxxtools/split.h>

#include "helper.hpp"
#include "structures.hpp"

using namespace std;

bool first = true;

void display_file(const File f) {
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
   printf("   export            Export database into json to stdout\n");
   printf("   import            Import database into json from stdin\n");
   printf("   diff +st1,-st2    Compares two storages\n");
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
         if(storage.empty()) {
            File::for_each(
               [](File f) { display_file(f); },
               "1 = 1 ORDER BY added DESC LIMIT :limit",
               [&limit](tntdb::Statement& st) { 
                   st.set("limit", limit); });
         } else {
            File::for_each(
               [](File f) { display_file(f); },
               "id IN (SELECT file_id FROM paths where storage = :st) "
               "ORDER BY added DESC LIMIT :limit",
               [&storage, &limit] (tntdb::Statement& st) {
                   st.set("st", storage).
                      set("limit", limit); });
         }
      }
      if(optind < argc && strcmp(argv[optind],"export")==0) {
         optind++;
         cxxtools::SerializationInfo si;
         cxxtools::JsonSerializer json(std::cout);

         cxxtools::SerializationInfo& fsi = si.addMember("files");
         File::for_each([&fsi](File f) {
                           cxxtools::SerializationInfo& nsi = fsi.addMember("");
                           f.serialize(nsi);
                       }, " movie_id = 0 ");
         fsi.setTypeName("set");
         fsi.setCategory(cxxtools::SerializationInfo::Array);

         cxxtools::SerializationInfo& msi = si.addMember("movies");
         Movie::for_each([&msi](Movie m) {
                           cxxtools::SerializationInfo& nsi = msi.addMember("");
                           m.serialize(nsi);
                       });
         msi.setTypeName("set");
         msi.setCategory(cxxtools::SerializationInfo::Array);

         json.beautify(true);   // this makes it just nice to read
         json.serialize(si);
         json.finish();
      }
      if(optind < argc && strcmp(argv[optind],"import")==0) {
         init_db();
         optind++;
         cxxtools::JsonDeserializer json(std::cin);
         json.deserialize();
         cxxtools::SerializationInfo* jsi = json.si();

         auto fsi = jsi->getMember("files");
         for(auto it : fsi)
            File::deserialize(it);

         auto msi = jsi->getMember("movies");
         for(auto it : msi)
            Movie::deserialize(it);
      }
      if(optind < argc && strcmp(argv[optind],"diff")==0) {
         optind++;
         if(optind >= argc) {
            print_help(argv[0]);
            exit(1);
         }
         std::vector<std::string> pargs;
         std::vector<std::string> nargs;
         {
         std::vector<std::string> args;
         cxxtools::split(",", std::string(argv[optind]), std::back_inserter(args));
         for(auto i : args) {
            if(i.empty())
               continue;
            if(i[0] == '-') {
               nargs.push_back(i.substr(1));
            } else if(i[0] == '+') {
               pargs.push_back(i.substr(1));
            } else {
               print_help(argv[0]);
               exit(1);
            }
         }
         }
         std::string where = "";
         char buff[128];

         if(!pargs.empty()) {
            where += " id IN (SELECT file_id FROM paths WHERE ";
            for(unsigned int i = 0; i < pargs.size(); i++) {
               if(i != 0) {
                  where += " OR ";
               }
               where += " storage = ";
               sprintf(buff, ":parg%u ", i);
               where += buff;
            }
            where += " ) ";
            if(!nargs.empty()) {
               where += " AND ";
            }
         }

         if(!nargs.empty()) {
            where += " id NOT IN (SELECT file_id FROM paths WHERE ";
            for(unsigned int i = 0; i < nargs.size(); i++) {
               if(i != 0) {
                  where += " OR ";
               }
               where += " storage = ";
               sprintf(buff, ":narg%u ", i);
               where += buff;
            }
            where += " ) ";
         }

         if(limit > 0)
            where += " LIMIT :limit";

         File::for_each(
            [](File f) { display_file(f); },
            where,
            [&nargs, &pargs, &limit] (tntdb::Statement& st) {
               char buff[128];

               for(unsigned int i = 0; i < nargs.size(); i++) {
                  sprintf(buff, "narg%u", i);
                  st.set(buff, nargs[i]);
               }

               for(unsigned int i = 0; i < pargs.size(); i++) {
                  sprintf(buff, "parg%u", i);
                  st.set(buff, pargs[i]);
               }

               st.set("limit", limit);
               });

         optind++;
      }
      if(optind < argc && strcmp(argv[optind],"search")==0) {
         optind++;
         if(optind >= argc) {
            print_help(argv[0]);
            exit(1);
         }
         std::string where = "path LIKE :name ";
         if(!storage.empty())
            where += " AND storage = :st ";
         if(limit > 0)
            where += " LIMIT :limit";
         Path::for_each(
            [](Path p) { display_file(p.get_parent_file()); },
            where,
            [&argv, &storage, &limit, &optind] (tntdb::Statement& st) {
                st.set("name", "%" + std::string(argv[optind]) + "%").
                   set("st", storage).
                   set("limit", limit); });
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
