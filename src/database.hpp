/**
 * \brief Definition of database object
 *
 * Database object is a singleton providing database abstraction. Should be
 * able to absorb structures and save them into database and should be able to
 * construct objects as a response to a (search) request.
 *
 */

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <vector>
#include <string>

#include "structures.hpp"

using namespace std;

//! Database abstraction class
class Database {
public:
   //! Singleton access method
   static Database* get_instance() {
      static Database* instance = NULL;
      if(instance == NULL)
         instance = new Database();
      return instance;
   }
   //! Takes object and saves it into database
   virtual void absorb(const File &target);
   //! Finds files matching filter and returns them
   virtual vector<File> get_files(
                                  //! Maximum number of files to return
                                  int max,
                                  //! Filter definition
                                  const string &filter = "");
};

//! Macro for easier access
#define DB Database::get_instance()

#endif // DATABASE_HPP
