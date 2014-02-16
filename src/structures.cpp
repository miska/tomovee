#include "structures.hpp"
#include "polarssl/md5.h"

File::File(const char* file, const string& storage) {
   paths.push_back(Path(storage,file));
   update_info(file);
}

void File::update_info(const char* file) {
   unsigned char sum[16];
   char buff[33];
   char* buff_ptr = buff;
   FILE* fl;

   if(md5_file(file, sum)==0) {
      for(int i=0; i<16; i++) {
         snprintf(buff_ptr, 3, "%02x", sum[i]);
         buff_ptr+=2;
      }
      buff[32] = 0;
      hash = buff;
   }
   fl = fopen(file,"r");
   fseek(fl, 0, SEEK_END);
   size = ftell(fl);
   fclose(fl);
}
