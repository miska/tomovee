/**
 * \brief Definition of database object
 *
 * Database object provides database abstraction layer.
 *
 */

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <vector>
#include "structures.hpp"

//! Database abstraction class
class Database {
public:
   
   //! Return all files from DB
   virtual vector<File> get_files() = 0;
   
   //! Changes paths of given files.
   /// if files do not exist they are created otherwise only paths are changed
   virtual void update_files(const vector<File>& files) = 0;

   //! Changes paths of given file.
   /// if file does not exist it is created otherwise only paths are changed
   virtual void update_file(const File& file) = 0;

   //! Returns pointer to the last created instance - pseudo singleton
   static Database* get_instance();

   virtual ~Database() {}
};

//! Factory method that returns sqlite backed implementation of Database
extern Database * open_sqlite_db(const string& filename);

#define DB (Database::get_instance())

#endif // DATABASE_HPP
