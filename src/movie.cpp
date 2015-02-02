/**
 * file movie.cpp
 *
 * brief Implementation of movie
 *
 * Movie abstraction class
 *
 */

/***** WARNING **************************************************************
 *                                                                          *
 * THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose *
 * your changes at the next build cycle. This is great for temporary printf *
 * statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places *
 * for commits are:                                                         *
 *                                                                          *
 * - The XML model used for this code generation: movie.xml, or             *
 * - The generation script that built this file: zxdb_cpp                   *
 *                                                                          *
 ***************************************************************************/

/*
  ==========================================================================
  ==========================================================================
*/

#include "movie.hpp"
#include "file.hpp"

bool Movie::table_exists = false;

extern std::string db_url;

//! Save object into database
void Movie::save() {
    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    // Update stuff in databse
    smt = conn.prepareCached("UPDATE movies SET "
                             "imdb_id = :imdb_id "
                             ","
                             "en_name = :en_name "
                             ","
                             "name = :name "
                             " WHERE id = :id");
    smt
        .set("imdb_id", imdb_id)
        .set("en_name", en_name)
        .set("name", name)
        .set("id", db_id)
        .execute();
}

//! Get all files belonging to this movie
std::vector<File> Movie::get_files() const {
    auto id = db_id;
    File::db_init();
    auto ret = File::search("movie_id = :movie_id",
        [id](tntdb::Statement& st) { st.set("movie_id", id); } );
    return ret;
}

//! Add new file to this movie
void Movie::add_file(File& file) {
    file.set_parent_movie_id(db_id);
}

//! Setter for imdb_id
std::string& Movie::set_imdb_id(const std::string val) {
    imdb_id = val;
    dirty = true;
    return imdb_id;
}

//! Getter for imdb_id
std::string  Movie::get_imdb_id() const {
    return imdb_id;
}

//! Setter for en_name
std::string& Movie::set_en_name(const std::string val) {
    en_name = val;
    dirty = true;
    return en_name;
}

//! Getter for en_name
std::string  Movie::get_en_name() const {
    return en_name;
}

//! Setter for name
std::string& Movie::set_name(const std::string val) {
    name = val;
    dirty = true;
    return name;
}

//! Getter for name
std::string  Movie::get_name() const {
    return name;
}

//! Equality comparison
bool Movie::operator==(const Movie& other) const {
    return
        imdb_id == (other.imdb_id) &&
        en_name == (other.en_name) &&
        name == (other.name);
}

//! Checks whether table exists and is correct
void Movie::db_init() {
    if(table_exists)
        return;

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    // Create database if does not exist
    smt = conn.prepare("CREATE TABLE IF NOT EXISTS movies ( "
                       "id INTEGER PRIMARY KEY AUTOINCREMENT "
                       ", imdb_id TEXT "
                       ", en_name TEXT "
                       ", name TEXT "
                       " ) ");
    smt.execute();

    table_exists = true;
}

//! Assignment operator
Movie& Movie::operator=(const Movie& other) {
    imdb_id = other.imdb_id;
    en_name = other.en_name;
    name = other.name;
    dirty = true;
    return (*this);
}

//! Exporting structure for future import
void Movie::serialize(cxxtools::SerializationInfo& si) {
    si.addMember("imdb_id").setValue(get_imdb_id());
    si.addMember("en_name").setValue(get_en_name());
    si.addMember("name").setValue(get_name());
    cxxtools::SerializationInfo& nsi = si.addMember("file");
    {
        auto list = get_files();
        for(auto i: list) {
            i.serialize(nsi.addMember(""));
        }
    }
    nsi.setTypeName("set");
    nsi.setCategory(cxxtools::SerializationInfo::Array);
}

//! Importing previously exported structure
Movie Movie::deserialize(const cxxtools::SerializationInfo& si) {
    std::string in_imdb_id;
    si.getMember("imdb_id") >>= in_imdb_id;
    std::string in_en_name;
    si.getMember("en_name") >>= in_en_name;
    std::string in_name;
    si.getMember("name") >>= in_name;
    Movie f(
        in_imdb_id,
        in_en_name,
        in_name
    );
    {
        auto msi = si.getMember("file");
        for(auto it : msi) {
            File tmp = File::deserialize(it);
            tmp.set_parent_movie(f);
        }
    }
    return f;
}

//! Default constructor
Movie::Movie(
            const std::string& in_imdb_id,
            const std::string& in_en_name,
            const std::string& in_name) {
    db_init();
    db_id = 0;
    dirty = false;
    imdb_id = in_imdb_id;
    en_name = in_en_name;
    name = in_name;

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;
    tntdb::Row row;

    // Add into database if doesn't exists
    smt = conn.prepareCached("INSERT INTO movies ( "
                             "imdb_id, "
                             "en_name, "
                             "name ) "
                             "SELECT "
                             ":imdb_id, "
                             ":en_name, "
                             ":name "
                             "WHERE 1 NOT IN "
                             "(SELECT 1 FROM movies WHERE "
                             "imdb_id = :imdb_id AND "
                             "en_name = :en_name AND "
                             "name = :name LIMIT 1)");
    smt
        .set("imdb_id", imdb_id)
        .set("en_name", en_name)
        .set("name", name)
        .execute();

    // Get ID from the database
    smt = conn.prepareCached("SELECT "
                             "  id "
                             " FROM movies WHERE "
                             "imdb_id = :imdb_id AND "
                             "en_name = :en_name AND "
                             "name = :name LIMIT 1");
    row = smt
        .set("imdb_id", imdb_id)
        .set("en_name", en_name)
        .set("name", name)
        .selectRow();
    row[0].get(db_id);
}

//! Runs specified function on every matching element
void Movie::for_each(std::function<void(Movie)> what,
        std::string where,
        std::function<void(tntdb::Statement&)> set) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "SELECT id "
                             ", imdb_id "
                             ", en_name "
                             ", name "
                             " FROM movies ";
    if(!where.empty()) {
        query += " WHERE ";
        query += where;
    }
    smt = conn.prepareCached(query);
    set(smt);

    // Construct response
    for(auto row: smt) {
        Movie it(
              row.getUnsigned64("id")
            , row.getString("imdb_id")
            , row.getString("en_name")
            , row.getString("name")
        );
        what(it);
    }
}

//! Returns element specified by it's database id
Movie Movie::get_by_id(uint64_t id) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "SELECT id "
                             ", imdb_id "
                             ", en_name "
                             ", name "
                             " FROM movies "
                             " WHERE id = :id ";
    smt = conn.prepareCached(query);
    smt.set("id", id);

    // Construct response
    auto row = smt.selectRow();
    Movie ret(
              row.getUnsigned64("id")
            , row.isNull("imdb_id") ? "" : row.getString("imdb_id")
            , row.isNull("en_name") ? "" : row.getString("en_name")
            , row.isNull("name") ? "" : row.getString("name")
        );
    return ret;
}

//! Deletes specified elements
void Movie::remove(std::string where,
    std::function<void(tntdb::Statement&)> set) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "DELETE FROM movies";
    if(!where.empty()) {
        query += " WHERE ";
        query += where;
    }
    smt = conn.prepareCached(query);
    set(smt);
    smt.execute();
}

