/**
 * \brief Implementation of objects used in the application
 */

#include "structures.hpp"
#include "polarssl/md5.h"

Path& Path::operator=(const Path& other) {
   storage=other.storage;
   path=other.path;
   return *this;
}

bool Path::operator==(const Path& b) {
   return ((storage == b.storage) && (path == b.path));
}

File::File(const char* file, const string& storage) {
   paths.push_back(Path(storage,file));
   update_info(file);
}

void File::update_info(const char* file) {
   unsigned char sum[16];
   char buff[33];
   char* buff_ptr = buff;
   FILE* fl;

   loaded = false;
   if((fl = fopen(file,"r")) == NULL) return;
   fseek(fl, 0, SEEK_END);
   size = ftell(fl);
   fclose(fl);
   if(md5_file(file, sum)==0) {
      for(int i=0; i<16; i++) {
         snprintf(buff_ptr, 3, "%02x", sum[i]);
         buff_ptr+=2;
      }
      buff[32] = 0;
      hash = buff;
   }
   loaded = true;
}

void File::assimilate(File& other) {
   if(looks_same(other)) {
      for(auto i : other.paths) {
         paths.push_back(Path(i));
      }
      other.paths.erase(other.paths.begin(), other.paths.end());
   }
}
