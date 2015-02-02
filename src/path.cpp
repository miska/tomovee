/**
 * file path.cpp
 *
 * brief Implementation of path
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

#include "path.hpp"
#include "file.hpp"

bool Path::table_exists = false;

extern std::string db_url;

//! Save object into database
void Path::save() {
    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    // Update stuff in databse
    smt = conn.prepareCached("UPDATE paths SET "
                             "storage = :storage "
                             ","
                             "path = :path "
                             ","
                             "checked = :checked "
                             ","
                             "file_id = :file_id "
                             " WHERE id = :id");
    smt
        .set("storage", storage)
        .set("path", path)
        .set("checked", checked)
        .set("file_id", file_id)
        .set("id", db_id)
        .execute();
}

//! Set Path::id of parent file
void Path::set_parent_file_id(uint64_t id) {
    file_id = id;
    dirty = true;
}

//! Get id of parent file
uint64_t Path::get_parent_file_id() const {
    return file_id;
}

//! Set parent file
void Path::set_parent_file(const File parent) {
    set_parent_file_id(parent.get_db_id());
}

//! Get parent file
File Path::get_parent_file() const {
    return File::get_by_id(get_parent_file_id());
}

//! Setter for storage
std::string& Path::set_storage(const std::string val) {
    storage = val;
    dirty = true;
    return storage;
}

//! Getter for storage
std::string  Path::get_storage() const {
    return storage;
}

//! Setter for path
std::string& Path::set_path(const std::string val) {
    path = val;
    dirty = true;
    return path;
}

//! Getter for path
std::string  Path::get_path() const {
    return path;
}

//! Setter for checked
uint64_t& Path::set_checked(const uint64_t val) {
    checked = val;
    dirty = true;
    return checked;
}

//! Getter for checked
uint64_t  Path::get_checked() const {
    return checked;
}

//! Equality comparison
bool Path::operator==(const Path& other) const {
    return
        storage == (other.storage) &&
        path == (other.path) &&
        checked == (other.checked);
}

//! Checks whether table exists and is correct
void Path::db_init() {
    if(table_exists)
        return;

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    // Create database if does not exist
    smt = conn.prepare("CREATE TABLE IF NOT EXISTS paths ( "
                       "id INTEGER PRIMARY KEY AUTOINCREMENT "
                       ", storage TEXT "
                       ", path TEXT "
                       ", checked BIGINT UNSIGNED "
                       ", file_id INTEGER DEFAULT 0"
                       " ) ");
    smt.execute();

    table_exists = true;
}

//! Assignment operator
Path& Path::operator=(const Path& other) {
    storage = other.storage;
    path = other.path;
    checked = other.checked;
    dirty = true;
    return (*this);
}

//! Exporting structure for future import
void Path::serialize(cxxtools::SerializationInfo& si) {
    si.addMember("storage").setValue(get_storage());
    si.addMember("path").setValue(get_path());
    si.addMember("checked").setValue(get_checked());
}

//! Importing previously exported structure
Path Path::deserialize(const cxxtools::SerializationInfo& si) {
    std::string in_storage;
    si.getMember("storage") >>= in_storage;
    std::string in_path;
    si.getMember("path") >>= in_path;
    uint64_t in_checked;
    si.getMember("checked") >>= in_checked;
    Path f(
        in_storage,
        in_path,
        in_checked
    );
    return f;
}

//! Default constructor
Path::Path(
            const std::string& in_storage,
            const std::string& in_path,
            const uint64_t& in_checked) {
    db_init();
    db_id = 0;
    dirty = false;
    storage = in_storage;
    path = in_path;
    checked = in_checked;

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;
    tntdb::Row row;

    // Add into database if doesn't exists
    smt = conn.prepareCached("INSERT INTO paths ( "
                             "storage, "
                             "path, "
                             "checked, "
                             "file_id) "
                             "SELECT "
                             ":storage, "
                             ":path, "
                             ":checked, "
                             "0 "
                             "WHERE 1 NOT IN "
                             "(SELECT 1 FROM paths WHERE "
                             "storage = :storage AND "
                             "path = :path AND "
                             "checked = :checked LIMIT 1)");
    smt
        .set("storage", storage)
        .set("path", path)
        .set("checked", checked)
        .execute();

    // Get ID from the database
    smt = conn.prepareCached("SELECT "
                             "  id "
                             ", file_id "
                             " FROM paths WHERE "
                             "storage = :storage AND "
                             "path = :path AND "
                             "checked = :checked LIMIT 1");
    row = smt
        .set("storage", storage)
        .set("path", path)
        .set("checked", checked)
        .selectRow();
    row[0].get(db_id);
    row[1].get(file_id);
}

//! Runs specified function on every matching element
void Path::for_each(std::function<void(Path)> what,
        std::string where,
        std::function<void(tntdb::Statement&)> set) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "SELECT id "
                             ", storage "
                             ", path "
                             ", checked "
                             ", file_id "
                             " FROM paths ";
    if(!where.empty()) {
        query += " WHERE ";
        query += where;
    }
    smt = conn.prepareCached(query);
    set(smt);

    // Construct response
    for(auto row: smt) {
        Path it(
              row.getUnsigned64("id")
            , row.getString("storage")
            , row.getString("path")
            , row.getUnsigned64("checked")
            , row.getUnsigned64("file_id")
        );
        what(it);
    }
}

//! Returns element specified by it's database id
Path Path::get_by_id(uint64_t id) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "SELECT id "
                             ", storage "
                             ", path "
                             ", checked "
                             ", file_id "
                             " FROM paths "
                             " WHERE id = :id ";
    smt = conn.prepareCached(query);
    smt.set("id", id);

    // Construct response
    auto row = smt.selectRow();
    Path ret(
              row.getUnsigned64("id")
            , row.isNull("storage") ? "" : row.getString("storage")
            , row.isNull("path") ? "" : row.getString("path")
            , row.getUnsigned64("checked")
            , row.getUnsigned64("file_id")
        );
    return ret;
}

//! Deletes specified elements
void Path::remove(std::string where,
    std::function<void(tntdb::Statement&)> set) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "DELETE FROM paths";
    if(!where.empty()) {
        query += " WHERE ";
        query += where;
    }
    smt = conn.prepareCached(query);
    set(smt);
    smt.execute();
}

