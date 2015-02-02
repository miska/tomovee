/**
 * file path.hpp
 *
 * brief Header for path
 *
 * Path abstraction class
 *
 */

/***** WARNING **************************************************************
 *                                                                          *
 * THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose *
 * your changes at the next build cycle. This is great for temporary printf *
 * statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places *
 * for commits are:                                                         *
 *                                                                          *
 * - The XML model used for this code generation: path.xml, or              *
 * - The generation script that built this file: zxdb_cpp                   *
 *                                                                          *
 ***************************************************************************/

/*
  ==========================================================================
  ==========================================================================
*/

#ifndef __PATH_H_INCLUDED__
#define __PATH_H_INCLUDED__

#include <vector>
#include <string>
#include <functional>
#include <tntdb.h>
#include <cxxtools/serializationinfo.h>


class File;

class Path {
private:
    //! Database id
    uint64_t db_id;
    //! Indicates changes not yet saved in database
    bool dirty;
    //! Indicates whether existence of the table was already checked
    static bool table_exists;
    //! What storage
    std::string storage;
    //! Relative path inside the storage
    std::string path;
    //! When was the last time we saw this file here
    uint64_t checked;
    //! File this path belongs to
    uint64_t file_id;
    //! Private constructor directly from database
    Path(
              const uint64_t in_db_id
            , const std::string in_storage
            , const std::string in_path
            , const uint64_t in_checked
            , const uint64_t in_file_id
            ) :
              db_id(in_db_id)
            , dirty(false)
            , storage(in_storage)
            , path(in_path)
            , checked(in_checked)
            , file_id(in_file_id)
            {}
public:
    //! Returns element specified by it's database id
    static Path get_by_id(uint64_t id);
    //! Destructor
    virtual ~Path() {
        if(dirty)
            save();
    }
    //! Makes sure that table exists (called from constructor)
    static void db_init();
    //! Tells class to reinitialize database (if you do something really nasty)
    static void db_reinit() {
        table_exists = false;
        db_init();
    }
    //! Save object into database
    virtual void save();
    //! Returns unique database id
    virtual uint64_t get_db_id() const { return db_id; }
    //! Setter for storage
    virtual std::string& set_storage(const std::string);
    //! Getter for storage
    virtual std::string  get_storage() const;
    //! Setter for path
    virtual std::string& set_path(const std::string);
    //! Getter for path
    virtual std::string  get_path() const;
    //! Setter for checked
    virtual uint64_t& set_checked(const uint64_t);
    //! Getter for checked
    virtual uint64_t  get_checked() const;
    //! Set id of parent file
    virtual void set_parent_file_id(uint64_t id);
    //! Get id of parent file
    virtual uint64_t get_parent_file_id() const;
    //! Set parent file
    virtual void set_parent_file(const File parent);
    //! Get parent file
    virtual File get_parent_file() const;
    //! Default constructor
    Path(
            const std::string& in_storage,
            const std::string& in_path,
            const uint64_t& in_checked = 0
        );
    //! Copy constructor
    Path(const Path& other) :
              db_id(other.db_id)
            , dirty(other.dirty)
            , storage(other.storage)
            , path(other.path)
            , checked(other.checked)
            , file_id(other.file_id)
            {}
    //! Move constructor
    Path(const Path&& other) :
              db_id(other.db_id)
            , dirty(other.dirty)
            , storage(move(other.storage))
            , path(move(other.path))
            , checked(other.checked)
            , file_id(other.file_id)
			{}
    //! Equality comparison
    virtual bool operator==(const Path& other) const;
    //! Assignment operator
    virtual Path& operator=(const Path& other);
    //! Inequality comparison
    bool operator!=(const Path& other) const {
        return !operator==(other);
    }
    //! Deletes specified elements
    static void remove(std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {});
    //! Runs specified function on every matching element
    static void for_each(std::function<void(Path)> what,
        std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {});
    //! Return matching instances
    static std::vector<Path> search(std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {}) {
        std::vector<Path> ret;
        Path::for_each([&ret](Path path) {
                ret.push_back(path);
            }, where, set);
        return ret;
    }
    //! Exporting structure for future import
    void serialize(cxxtools::SerializationInfo& si);
    //! Importing previously exported structure
    static Path deserialize(const cxxtools::SerializationInfo& si);
};

#endif
