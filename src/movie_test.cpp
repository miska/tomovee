/**
 * file movie_test.cpp
 *
 * brief Tests for movie
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

#include <string>
#include <stdio.h>
#include <unistd.h>
#include "catch.hpp"
#include "movie.hpp"
#include "file.hpp"

extern std::string db_url;

TEST_CASE( "Movie works", "[movie]" ) {
    db_url  = "sqlite:";
    db_url += tmpnam(NULL);
    unlink(db_url.c_str());
    Movie::db_reinit();
    File::db_reinit();

    SECTION("Check that getters works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        REQUIRE(test_a.get_db_id() != 0);
    }

    SECTION("Check that getters works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        REQUIRE(test_a.get_imdb_id() == "a");
        REQUIRE(test_a.get_en_name() == "a");
        REQUIRE(test_a.get_name() == "a");
    }
    SECTION("Check that setters works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        test_a.set_imdb_id("b");
        test_a.set_en_name("b");
        test_a.set_name("b");
        REQUIRE(test_a.get_imdb_id() == "b");
        REQUIRE(test_a.get_en_name() == "b");
        REQUIRE(test_a.get_name() == "b");
    }

    SECTION("Check that comparison works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        Movie test_b(
            "b",
            "b",
            "b"
        );
        REQUIRE(test_a != test_b);
        test_a.set_imdb_id("b");
        test_a.set_en_name("b");
        test_a.set_name("b");
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that assignment works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        Movie test_b(
            "b",
            "b",
            "b"
        );
        REQUIRE(test_a != test_b);
        test_a = test_b;
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that copy constructor works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        Movie test_b(test_a);
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that search works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        Movie test_b(
            "b",
            "b",
            "b"
        );
        std::vector<Movie> res;
        res = Movie::search();
        REQUIRE(res.size() == 2);
        res = Movie::search(
            "imdb_id = 'b' AND "
            "en_name = 'b' AND "
            "name = 'b'"
        );
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == test_b);
        res = Movie::search(
            "imdb_id = :str AND "
            "en_name = :str AND "
            "name = :str"
            ,
            [](tntdb::Statement& st) {
                st.set("str", "b").set("num", 2);
            }
        );
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == test_b);
    }

    SECTION("Check that setters works reliably") {
        {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        test_a.set_imdb_id("b");
        test_a.set_en_name("b");
        test_a.set_name("b");
        }
        auto res = Movie::search(
            "imdb_id = :str AND "
            "en_name = :str AND "
            "name = :str"
            ,
            [](tntdb::Statement& st) {
                st.set("str", "b").set("num", 2);
            }
        );
        REQUIRE(res.size() == 1);
    }

    SECTION("Check that remove works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        Movie test_b(
            "b",
            "b",
            "b"
        );
        std::vector<Movie> res;
        res = Movie::search();
        REQUIRE(res.size() == 2);
        Movie::remove(
            "imdb_id = 'b' AND "
            "en_name = 'b' AND "
            "name = 'b'"
        );
        res = Movie::search();
        REQUIRE(res.size() == 1);
        Movie::remove();
        res = Movie::search();
        REQUIRE(res.size() == 0);
    }

    SECTION("Getting members works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        REQUIRE(test_a.get_files().size() == 0);
    }

    SECTION("Serialization/deserialization works") {
        Movie test_a(
            "a",
            "a",
            "a"
        );
        cxxtools::SerializationInfo si;
        test_a.serialize(si);
        Movie test_b = Movie::deserialize(si);
        REQUIRE(test_a == test_b);
    }

    SECTION("Get by id works with empty strings") {
        Movie test_a(
            "",
            "",
            ""
        );
        Movie test_b = Movie::get_by_id(test_a.get_db_id());
        REQUIRE(test_a == test_b);
    }

    unlink(db_url.c_str());
}

