/**
 * file file.cpp
 *
 * brief Implementation of file
 *
 * File abstraction class
 *
 */

/***** WARNING **************************************************************
 *                                                                          *
 * THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose *
 * your changes at the next build cycle. This is great for temporary printf *
 * statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places *
 * for commits are:                                                         *
 *                                                                          *
 * - The XML model used for this code generation: file.xml, or              *
 * - The generation script that built this file: zxdb_cpp                   *
 *                                                                          *
 ***************************************************************************/

/*
  ==========================================================================
  ==========================================================================
*/

#include "file.hpp"
#include "path.hpp"
#include "movie.hpp"

bool File::table_exists = false;

extern std::string db_url;

//! Save object into database
void File::save() {
    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    // Update stuff in databse
    smt = conn.prepareCached("UPDATE files SET "
                             "size = :size "
                             ","
                             "mhash = :mhash "
                             ","
                             "osdbhash = :osdbhash "
                             ","
                             "added = :added "
                             ","
                             "audios = :audios "
                             ","
                             "subtitles = :subtitles "
                             ","
                             "width = :width "
                             ","
                             "height = :height "
                             ","
                             "length = :length "
                             ","
                             "movie_assigned_by = :movie_assigned_by "
                             ","
                             "movie_id = :movie_id "
                             " WHERE id = :id");
    smt
        .set("size", size)
        .set("mhash", mhash)
        .set("osdbhash", osdbhash)
        .set("added", added)
        .set("audios", audios)
        .set("subtitles", subtitles)
        .set("width", width)
        .set("height", height)
        .set("length", length)
        .set("movie_assigned_by", movie_assigned_by)
        .set("movie_id", movie_id)
        .set("id", db_id)
        .execute();
}

//! Get all paths belonging to this file
std::vector<Path> File::get_paths() const {
    auto id = db_id;
    Path::db_init();
    auto ret = Path::search("file_id = :file_id",
        [id](tntdb::Statement& st) { st.set("file_id", id); } );
    return ret;
}

//! Add new path to this file
void File::add_path(Path& path) {
    path.set_parent_file_id(db_id);
}

//! Set File::id of parent movie
void File::set_parent_movie_id(uint64_t id) {
    movie_id = id;
    dirty = true;
}

//! Get id of parent movie
uint64_t File::get_parent_movie_id() const {
    return movie_id;
}

//! Set parent movie
void File::set_parent_movie(const Movie parent) {
    set_parent_movie_id(parent.get_db_id());
}

//! Get parent movie
Movie File::get_parent_movie() const {
    return Movie::get_by_id(get_parent_movie_id());
}

//! Setter for size
int64_t& File::set_size(const int64_t val) {
    size = val;
    dirty = true;
    return size;
}

//! Getter for size
int64_t   File::get_size() const {
    return size;
}

//! Setter for mhash
uint32_t& File::set_mhash(const uint32_t val) {
    mhash = val;
    dirty = true;
    return mhash;
}

//! Getter for mhash
uint32_t  File::get_mhash() const {
    return mhash;
}

//! Setter for osdbhash
uint64_t& File::set_osdbhash(const uint64_t val) {
    osdbhash = val;
    dirty = true;
    return osdbhash;
}

//! Getter for osdbhash
uint64_t  File::get_osdbhash() const {
    return osdbhash;
}

//! Setter for added
uint64_t& File::set_added(const uint64_t val) {
    added = val;
    dirty = true;
    return added;
}

//! Getter for added
uint64_t  File::get_added() const {
    return added;
}

//! Setter for audios
std::string& File::set_audios(const std::string val) {
    audios = val;
    dirty = true;
    return audios;
}

//! Getter for audios
std::string  File::get_audios() const {
    return audios;
}

//! Setter for subtitles
std::string& File::set_subtitles(const std::string val) {
    subtitles = val;
    dirty = true;
    return subtitles;
}

//! Getter for subtitles
std::string  File::get_subtitles() const {
    return subtitles;
}

//! Setter for width
uint32_t& File::set_width(const uint32_t val) {
    width = val;
    dirty = true;
    return width;
}

//! Getter for width
uint32_t  File::get_width() const {
    return width;
}

//! Setter for height
uint32_t& File::set_height(const uint32_t val) {
    height = val;
    dirty = true;
    return height;
}

//! Getter for height
uint32_t  File::get_height() const {
    return height;
}

//! Setter for length
uint32_t& File::set_length(const uint32_t val) {
    length = val;
    dirty = true;
    return length;
}

//! Getter for length
uint32_t  File::get_length() const {
    return length;
}

//! Setter for movie_assigned_by
uint64_t& File::set_movie_assigned_by(const uint64_t val) {
    movie_assigned_by = val;
    dirty = true;
    return movie_assigned_by;
}

//! Getter for movie_assigned_by
uint64_t  File::get_movie_assigned_by() const {
    return movie_assigned_by;
}

//! Equality comparison
bool File::operator==(const File& other) const {
    return
        size == (other.size) &&
        mhash == (other.mhash) &&
        osdbhash == (other.osdbhash) &&
        added == (other.added) &&
        audios == (other.audios) &&
        subtitles == (other.subtitles) &&
        width == (other.width) &&
        height == (other.height) &&
        length == (other.length) &&
        movie_assigned_by == (other.movie_assigned_by);
}

//! Checks whether table exists and is correct
void File::db_init() {
    if(table_exists)
        return;

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    // Create database if does not exist
    smt = conn.prepare("CREATE TABLE IF NOT EXISTS files ( "
                       "id INTEGER PRIMARY KEY AUTOINCREMENT "
                       ", size BIGINT "
                       ", mhash INTEGER UNSIGNED "
                       ", osdbhash BIGINT UNSIGNED "
                       ", added BIGINT UNSIGNED "
                       ", audios TEXT "
                       ", subtitles TEXT "
                       ", width INTEGER UNSIGNED "
                       ", height INTEGER UNSIGNED "
                       ", length INTEGER UNSIGNED "
                       ", movie_assigned_by SMALLINT UNSIGNED "
                       ", movie_id INTEGER DEFAULT 0"
                       " ) ");
    smt.execute();

    table_exists = true;
}

//! Assignment operator
File& File::operator=(const File& other) {
    size = other.size;
    mhash = other.mhash;
    osdbhash = other.osdbhash;
    added = other.added;
    audios = other.audios;
    subtitles = other.subtitles;
    width = other.width;
    height = other.height;
    length = other.length;
    movie_assigned_by = other.movie_assigned_by;
    dirty = true;
    return (*this);
}

//! Exporting structure for future import
void File::serialize(cxxtools::SerializationInfo& si) {
    si.addMember("size").setValue(get_size());
    si.addMember("mhash").setValue(get_mhash());
    si.addMember("osdbhash").setValue(get_osdbhash());
    si.addMember("added").setValue(get_added());
    si.addMember("audios").setValue(get_audios());
    si.addMember("subtitles").setValue(get_subtitles());
    si.addMember("width").setValue(get_width());
    si.addMember("height").setValue(get_height());
    si.addMember("length").setValue(get_length());
    si.addMember("movie_assigned_by").setValue(get_movie_assigned_by());
    cxxtools::SerializationInfo& nsi = si.addMember("path");
    {
        auto list = get_paths();
        for(auto i: list) {
            i.serialize(nsi.addMember(""));
        }
    }
    nsi.setTypeName("set");
    nsi.setCategory(cxxtools::SerializationInfo::Array);
}

//! Importing previously exported structure
File File::deserialize(const cxxtools::SerializationInfo& si) {
    int64_t in_size;
    si.getMember("size") >>= in_size;
    uint32_t in_mhash;
    si.getMember("mhash") >>= in_mhash;
    uint64_t in_osdbhash;
    si.getMember("osdbhash") >>= in_osdbhash;
    uint64_t in_added;
    si.getMember("added") >>= in_added;
    std::string in_audios;
    si.getMember("audios") >>= in_audios;
    std::string in_subtitles;
    si.getMember("subtitles") >>= in_subtitles;
    uint32_t in_width;
    si.getMember("width") >>= in_width;
    uint32_t in_height;
    si.getMember("height") >>= in_height;
    uint32_t in_length;
    si.getMember("length") >>= in_length;
    uint64_t in_movie_assigned_by;
    si.getMember("movie_assigned_by") >>= in_movie_assigned_by;
    File f(
        in_size,
        in_mhash,
        in_osdbhash,
        in_added,
        in_audios,
        in_subtitles,
        in_width,
        in_height,
        in_length,
        in_movie_assigned_by
    );
    {
        auto msi = si.getMember("path");
        for(auto it : msi) {
            Path tmp = Path::deserialize(it);
            tmp.set_parent_file(f);
        }
    }
    return f;
}

//! Default constructor
File::File(
            const int64_t& in_size,
            const uint32_t& in_mhash,
            const uint64_t& in_osdbhash,
            const uint64_t& in_added,
            const std::string& in_audios,
            const std::string& in_subtitles,
            const uint32_t& in_width,
            const uint32_t& in_height,
            const uint32_t& in_length,
            const uint64_t& in_movie_assigned_by) {
    db_init();
    db_id = 0;
    dirty = false;
    size = in_size;
    mhash = in_mhash;
    osdbhash = in_osdbhash;
    added = in_added;
    audios = in_audios;
    subtitles = in_subtitles;
    width = in_width;
    height = in_height;
    length = in_length;
    movie_assigned_by = in_movie_assigned_by;

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;
    tntdb::Row row;

    // Add into database if doesn't exists
    smt = conn.prepareCached("INSERT INTO files ( "
                             "size, "
                             "mhash, "
                             "osdbhash, "
                             "added, "
                             "audios, "
                             "subtitles, "
                             "width, "
                             "height, "
                             "length, "
                             "movie_assigned_by, "
                             "movie_id) "
                             "SELECT "
                             ":size, "
                             ":mhash, "
                             ":osdbhash, "
                             ":added, "
                             ":audios, "
                             ":subtitles, "
                             ":width, "
                             ":height, "
                             ":length, "
                             ":movie_assigned_by, "
                             "0 "
                             "WHERE 1 NOT IN "
                             "(SELECT 1 FROM files WHERE "
                             "size = :size AND "
                             "mhash = :mhash AND "
                             "osdbhash = :osdbhash AND "
                             "added = :added AND "
                             "audios = :audios AND "
                             "subtitles = :subtitles AND "
                             "width = :width AND "
                             "height = :height AND "
                             "length = :length AND "
                             "movie_assigned_by = :movie_assigned_by LIMIT 1)");
    smt
        .set("size", size)
        .set("mhash", mhash)
        .set("osdbhash", osdbhash)
        .set("added", added)
        .set("audios", audios)
        .set("subtitles", subtitles)
        .set("width", width)
        .set("height", height)
        .set("length", length)
        .set("movie_assigned_by", movie_assigned_by)
        .execute();

    // Get ID from the database
    smt = conn.prepareCached("SELECT "
                             "  id "
                             ", movie_id "
                             " FROM files WHERE "
                             "size = :size AND "
                             "mhash = :mhash AND "
                             "osdbhash = :osdbhash AND "
                             "added = :added AND "
                             "audios = :audios AND "
                             "subtitles = :subtitles AND "
                             "width = :width AND "
                             "height = :height AND "
                             "length = :length AND "
                             "movie_assigned_by = :movie_assigned_by LIMIT 1");
    row = smt
        .set("size", size)
        .set("mhash", mhash)
        .set("osdbhash", osdbhash)
        .set("added", added)
        .set("audios", audios)
        .set("subtitles", subtitles)
        .set("width", width)
        .set("height", height)
        .set("length", length)
        .set("movie_assigned_by", movie_assigned_by)
        .selectRow();
    db_id = 0;
    row[0].get(db_id);
    movie_id = 0;
    row[1].get(movie_id);
}

//! Runs specified function on every matching element
void File::for_each(std::function<void(File)> what,
        std::string where,
        std::function<void(tntdb::Statement&)> set) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "SELECT id "
                             ", size "
                             ", mhash "
                             ", osdbhash "
                             ", added "
                             ", audios "
                             ", subtitles "
                             ", width "
                             ", height "
                             ", length "
                             ", movie_assigned_by "
                             ", movie_id "
                             " FROM files ";
    if(!where.empty()) {
        query += " WHERE ";
        query += where;
    }
    smt = conn.prepareCached(query);
    set(smt);

    // Construct response
    for(auto row: smt) {
        File it(
              row.getUnsigned64("id")
            , row.getInt64("size")
            , row.getUnsigned32("mhash")
            , row.getUnsigned64("osdbhash")
            , row.getUnsigned64("added")
            , row.getString("audios")
            , row.getString("subtitles")
            , row.getUnsigned32("width")
            , row.getUnsigned32("height")
            , row.getUnsigned32("length")
            , row.getUnsigned32("movie_assigned_by")
            , row.getUnsigned64("movie_id")
        );
        what(it);
    }
}

//! Returns element specified by it's database id
File File::get_by_id(uint64_t id) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "SELECT id "
                             ", size "
                             ", mhash "
                             ", osdbhash "
                             ", added "
                             ", audios "
                             ", subtitles "
                             ", width "
                             ", height "
                             ", length "
                             ", movie_assigned_by "
                             ", movie_id "
                             " FROM files "
                             " WHERE id = :id ";
    smt = conn.prepareCached(query);
    smt.set("id", id);

    // Construct response
    auto row = smt.selectRow();
    File ret(
              row.getUnsigned64("id")
            , row.getInt64("size")
            , row.getUnsigned32("mhash")
            , row.getUnsigned64("osdbhash")
            , row.getUnsigned64("added")
            , row.isNull("audios") ? "" : row.getString("audios")
            , row.isNull("subtitles") ? "" : row.getString("subtitles")
            , row.getUnsigned32("width")
            , row.getUnsigned32("height")
            , row.getUnsigned32("length")
            , row.getUnsigned32("movie_assigned_by")
            , row.getUnsigned64("movie_id")
        );
    return ret;
}

//! Deletes specified elements
void File::remove(std::string where,
    std::function<void(tntdb::Statement&)> set) {

    tntdb::Connection conn = tntdb::connectCached(db_url);
    tntdb::Statement smt;

    db_init();

    // Query data
    std::string query = "DELETE FROM files";
    if(!where.empty()) {
        query += " WHERE ";
        query += where;
    }
    smt = conn.prepareCached(query);
    set(smt);
    smt.execute();
}

