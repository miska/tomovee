/**
 * file movie.hpp
 *
 * brief Header for movie
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

#ifndef __MOVIE_H_INCLUDED__
#define __MOVIE_H_INCLUDED__

#include <vector>
#include <string>
#include <functional>
#include <tntdb.h>
#include <cxxtools/serializationinfo.h>


class File;

class Movie {
private:
    //! Database id
    uint64_t db_id;
    //! Indicates changes not yet saved in database
    bool dirty;
    //! Indicates whether existence of the table was already checked
    static bool table_exists;
    //! ID of this movie on IMDB
    std::string imdb_id;
    //! English name of the movie
    std::string en_name;
    //! Native name of the movie
    std::string name;
    //! Private constructor directly from database
    Movie(
              const uint64_t in_db_id
            , const std::string in_imdb_id
            , const std::string in_en_name
            , const std::string in_name
            ) :
              db_id(in_db_id)
            , dirty(false)
            , imdb_id(in_imdb_id)
            , en_name(in_en_name)
            , name(in_name)
            {}
public:
    //! Returns element specified by it's database id
    static Movie get_by_id(uint64_t id);
    //! Destructor
    virtual ~Movie() {
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
    //! Setter for imdb_id
    virtual std::string& set_imdb_id(const std::string);
    //! Getter for imdb_id
    virtual std::string  get_imdb_id() const;
    //! Setter for en_name
    virtual std::string& set_en_name(const std::string);
    //! Getter for en_name
    virtual std::string  get_en_name() const;
    //! Setter for name
    virtual std::string& set_name(const std::string);
    //! Getter for name
    virtual std::string  get_name() const;
    //! Get all files belonging to this movie
    virtual std::vector<File> get_files() const;
    //! Add new file to this movie
    virtual void add_file(File& file);
    //! Default constructor
    Movie(
            const std::string& in_imdb_id,
            const std::string& in_en_name = "",
            const std::string& in_name = ""
        );
    //! Copy constructor
    Movie(const Movie& other) :
              db_id(other.db_id)
            , dirty(other.dirty)
            , imdb_id(other.imdb_id)
            , en_name(other.en_name)
            , name(other.name)
            {}
    //! Move constructor
    Movie(const Movie&& other) :
              db_id(other.db_id)
            , dirty(other.dirty)
            , imdb_id(move(other.imdb_id))
            , en_name(move(other.en_name))
            , name(move(other.name))
            {}
    //! Equality comparison
    virtual bool operator==(const Movie& other) const;
    //! Assignment operator
    virtual Movie& operator=(const Movie& other);
    //! Inequality comparison
    bool operator!=(const Movie& other) const {
        return !operator==(other);
    }
    //! Deletes specified elements
    static void remove(std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {});
    //! Runs specified function on every matching element
    static void for_each(std::function<void(Movie)> what,
        std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {});
    //! Return matching instances
    static std::vector<Movie> search(std::string where = "",
        std::function<void(tntdb::Statement&)> set = [](tntdb::Statement&) {}) {
        std::vector<Movie> ret;
        Movie::for_each([&ret](Movie movie) {
                ret.push_back(movie);
            }, where, set);
        return ret;
    }
    //! Exporting structure for future import
    void serialize(cxxtools::SerializationInfo& si);
    //! Importing previously exported structure
    static Movie deserialize(const cxxtools::SerializationInfo& si);
};

#endif
