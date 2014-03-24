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
   /// if file does not exist it created otherwise only its paths are changed
   virtual void update_files(const vector<File>& files) = 0;
   
   virtual ~Database() {}
};

//! Factory method that returns sqlite backed implementation of Database
extern Database * open_sqlite_db(const string& filename);

#endif // DATABASE_HPP
