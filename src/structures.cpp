/**
 * \brief Implementation of objects used in the application
 */

#include "structures.hpp"
#include "helper.hpp"

#include <tntdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//! Update movie meta data like resolution, streams and length
void update_file_meta(File& target, const char* file) {
   if((target.get_length() > 0) && (target.get_width() > 0) && (target.get_height() > 0))
      return;
   std::string a,s;
   int32_t l,w,h;
   get_movie_info(file, l, w, h, a, s);
   if(target.get_audios().empty())
      target.set_audios(a);
   if(target.get_subtitles().empty())
      target.set_subtitles(s);
   target.set_length(l);
   target.set_width(w);
   target.set_height(h);
}

//! Update last checked time
void touch_path(Path& what,time_t when) {
   uint64_t checked;
   if(when == 0)
      checked = time(NULL);
   else
      checked = when;

   what.set_checked(max(checked,what.get_checked()));
}

//! Create File class from file
File create_file(const char* storage, const char* file, bool use_mtime) {
   FILE* fl;

   // Set size
   uint64_t size = 0;
   if((fl = fopen(file,"r")) == NULL)
      throw std::runtime_error("Can't open the file");
   fseek(fl, 0, SEEK_END);
   size = ftell(fl);

   // Calculate hashes
   uint64_t osdbhash = 0;
   uint32_t mhash = 0;
   compute_hash(osdbhash, mhash, fl);
   fclose(fl);

   // Get/create a file
   auto search_fl = File::search(
      "osdbhash = :osdb AND mhash = :mhash AND size = :size",
      [&osdbhash, &mhash, &size](tntdb::Statement& st) {
         st.setUnsigned64("osdb", osdbhash);
         st.setUnsigned32("mhash", mhash);
         st.setUnsigned64("size", size);
      }
   );
   if(search_fl.empty())
      search_fl.push_back(File(size, mhash, osdbhash));

   // Get/create a path
   {
   auto search_pt = Path::search(
      "storage = :st AND path = :pt",
      [&storage, &file](tntdb::Statement& st) {
         st.set("st", storage);
         st.set("pt", file);
      }
   );
   if(search_pt.empty()) {
      Path pt(storage, file);
      pt.set_parent_file(search_fl.front());
   } else {
      touch_path(search_pt.front());
      search_pt.front().set_parent_file(search_fl.front());
   }
   }

   // Update info
   update_file_info(search_fl.front(), file, use_mtime);
   return search_fl.front();
}

//! Update hash and filesize
void update_file_info(File& target, const char* file, bool use_mtime) {

   // Set time
   uint64_t added = 0;
   if(use_mtime) {
      struct stat st;
      if(stat(file, &st) == 0) {
         added = st.st_mtime;
      }
   }
   if(added == 0) {
      added = time(NULL);
   }
   target.set_added(min(target.get_added(),added));
   if(target.get_added() == 0)
      target.set_added(added);

   update_file_meta(target, file);
}
