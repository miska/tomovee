/**
 * file file.hpp
 *
 * brief Header for file
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

#ifndef __FILE_H_INCLUDED__
#define __FILE_H_INCLUDED__

#include <vector>
#include <string>
#include <functional>
#include <tntdb.h>
#include <cxxtools/serializationinfo.h>


class Path;
class Movie;

class File {
private:
    //! Database id
    uint64_t db_id;
    //! Indicates changes not yet saved in database
    bool dirty;
    //! Indicates whether existence of the table was already checked
    static bool table_exists;
    //! Size of the file
    int64_t size;
    //! Hash from the middle of the movie
    uint32_t mhash;
    //! Hash used by OpenSubtitles
    uint64_t osdbhash;
    //! Time when the file was added
    uint64_t added;
    //! Comma separated list of audio tracks
    std::string audios;
    //! Comma separated list of subtitle tracks
    std::string subtitles;
    //! Movie width
    uint32_t width;
    //! Movie height
    uint32_t height;
    //! Movie length in seconds
    uint32_t length;
    //! How did we get the link to the movie
    uint64_t movie_assigned_by;
    //! Movie this file contains
    uint64_t movie_id;
    //! Private constructor directly from database
    File(
              const uint64_t in_db_id
            , const int64_t in_size
            , const uint32_t in_mhash
            , const uint64_t in_osdbhash
            , const uint64_t in_added
            , const std::string in_audios
            , const std::string in_subtitles
            , const uint32_t in_width
            , const uint32_t in_height
            , const uint32_t in_length
            , const uint64_t in_movie_assigned_by
            , const uint64_t in_movie_id
            ) :
              db_id(in_db_id)
            , dirty(false)
            , size(in_size)
            , mhash(in_mhash)
            , osdbhash(in_osdbhash)
            , added(in_added)
            , audios(in_audios)
            , subtitles(in_subtitles)
            , width(in_width)
            , height(in_height)
            , length(in_length)
            , movie_assigned_by(in_movie_assigned_by)
            , movie_id(in_movie_id)
            {}
public:
    //! Returns element specified by it's database id
    static File get_by_id(uint64_t id);
    //! Destructor
    virtual ~File() {
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
    //! Setter for size
    virtual int64_t& set_size(const int64_t);
    //! Getter for size
    virtual int64_t   get_size() const;
    //! Setter for mhash
    virtual uint32_t& set_mhash(const uint32_t);
    //! Getter for mhash
    virtual uint32_t  get_mhash() const;
    //! Setter for osdbhash
    virtual uint64_t& set_osdbhash(const uint64_t);
    //! Getter for osdbhash
    virtual uint64_t  get_osdbhash() const;
    //! Setter for added
    virtual uint64_t& set_added(const uint64_t);
    //! Getter for added
    virtual uint64_t  get_added() const;
    //! Setter for audios
    virtual std::string& set_audios(const std::string);
    //! Getter for audios
    virtual std::string  get_audios() const;
    //! Setter for subtitles
    virtual std::string& set_subtitles(const std::string);
    //! Getter for subtitles
    virtual std::string  get_subtitles() const;
    //! Setter for width
    virtual uint32_t& set_width(const uint32_t);
    //! Getter for width
    virtual uint32_t  get_width() const;
    //! Setter for height
    virtual uint32_t& set_height(const uint32_t);
    //! Getter for height
    virtual uint32_t  get_height() const;
    //! Setter for length
    virtual uint32_t& set_length(const uint32_t);
    //! Getter for length
    virtual uint32_t  get_length() const;
    //! Setter for movie_assigned_by
    virtual uint64_t& set_movie_assigned_by(const uint64_t);
    //! Getter for movie_assigned_by
    virtual uint64_t  get_movie_assigned_by() const;
    //! Get all paths belonging to this file
    virtual std::vector<Path> get_paths() const;
    //! Add new path to this file
    virtual void add_path(Path& path);
    //! Set id of parent movie
    virtual void set_parent_movie_id(uint64_t id);
    //! Get id of parent movie
    virtual uint64_t get_parent_movie_id() const;
    //! Set parent movie
    virtual void set_parent_movie(const Movie parent);
    //! Get parent movie
    virtual Movie get_parent_movie() const;
    //! Default constructor
    File(
            const int64_t& in_size,
            const uint32_t& in_mhash,
            const uint64_t& in_osdbhash,
            const uint64_t& in_added = time(NULL),
            const std::string& in_audios = "",
            const std::string& in_subtitles = "",
            const uint32_t& in_width = 0,
            const uint32_t& in_height = 0,
            const uint32_t& in_length = 0,
            const uint64_t& in_movie_assigned_by = 0
        );
    //! Copy constructor
    File(const File& other) :
              db_id(other.db_id)
            , dirty(other.dirty)
            , size(other.size)
            , mhash(other.mhash)
            , osdbhash(other.osdbhash)
            , added(other.added)
            , audios(other.audios)
            , subtitles(other.subtitles)
            , width(other.width)
            , height(other.height)
            , length(other.length)
            , movie_assigned_by(other.movie_assigned_by)
            , movie_id(other.movie_id)
            {}
    //! Move constructor
    File(const File&& other) :
              db_id(other.db_id)
            , dirty(other.dirty)
            , size(other.size)
            , mhash(other.mhash)
            , osdbhash(other.osdbhash)
            , added(other.added)
            , audios(move(other.audios))
            , subtitles(move(other.subtitles))
            , width(other.width)
            , height(other.height)
            , length(other.length)
            , movie_assigned_by(other.movie_assigned_by)
            , movie_id(other.movie_id)
			{}
    //! Equality comparison
    virtual bool operator==(const File& other) const;
    //! Assignment operator
    virtual File& operator=(const File& other);
    //! Inequality comparison
    bool operator!=(const File& other) const {
        return !operator==(other);
    }
    //! Deletes specified elements
    static void remove(std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {});
    //! Runs specified function on every matching element
    static void for_each(std::function<void(File)> what,
        std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {});
    //! Return matching instances
    static std::vector<File> search(std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {}) {
        std::vector<File> ret;
        File::for_each([&ret](File file) {
                ret.push_back(file);
            }, where, set);
        return ret;
    }
    //! Exporting structure for future import
    void serialize(cxxtools::SerializationInfo& si);
    //! Importing previously exported structure
    static File deserialize(const cxxtools::SerializationInfo& si);
};

#endif
