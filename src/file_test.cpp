/**
 * file file_test.cpp
 *
 * brief Tests for file
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

#include <string>
#include <stdio.h>
#include <unistd.h>
#include "catch.hpp"
#include "file.hpp"
#include "path.hpp"

extern std::string db_url;

TEST_CASE( "File works", "[file]" ) {
    db_url  = "sqlite:";
    db_url += tmpnam(NULL);
    unlink(db_url.c_str());
    File::db_reinit();
    Path::db_reinit();

    SECTION("Check that getters works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        REQUIRE(test_a.get_db_id() != 0);
    }

    SECTION("Check that getters works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        REQUIRE(test_a.get_size() == 1);
        REQUIRE(test_a.get_mhash() == 1);
        REQUIRE(test_a.get_osdbhash() == 1);
        REQUIRE(test_a.get_added() == 1);
        REQUIRE(test_a.get_audios() == "a");
        REQUIRE(test_a.get_subtitles() == "a");
        REQUIRE(test_a.get_width() == 1);
        REQUIRE(test_a.get_height() == 1);
        REQUIRE(test_a.get_length() == 1);
        REQUIRE(test_a.get_movie_assigned_by() == 1);
    }
    SECTION("Check that setters works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        test_a.set_size(2);
        test_a.set_mhash(2);
        test_a.set_osdbhash(2);
        test_a.set_added(2);
        test_a.set_audios("b");
        test_a.set_subtitles("b");
        test_a.set_width(2);
        test_a.set_height(2);
        test_a.set_length(2);
        test_a.set_movie_assigned_by(2);
        REQUIRE(test_a.get_size() == 2);
        REQUIRE(test_a.get_mhash() == 2);
        REQUIRE(test_a.get_osdbhash() == 2);
        REQUIRE(test_a.get_added() == 2);
        REQUIRE(test_a.get_audios() == "b");
        REQUIRE(test_a.get_subtitles() == "b");
        REQUIRE(test_a.get_width() == 2);
        REQUIRE(test_a.get_height() == 2);
        REQUIRE(test_a.get_length() == 2);
        REQUIRE(test_a.get_movie_assigned_by() == 2);
    }

    SECTION("Check that comparison works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        File test_b(
            2,
            2,
            2,
            2,
            "b",
            "b",
            2,
            2,
            2,
            2
        );
        REQUIRE(test_a != test_b);
        test_a.set_size(2);
        test_a.set_mhash(2);
        test_a.set_osdbhash(2);
        test_a.set_added(2);
        test_a.set_audios("b");
        test_a.set_subtitles("b");
        test_a.set_width(2);
        test_a.set_height(2);
        test_a.set_length(2);
        test_a.set_movie_assigned_by(2);
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that assignment works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        File test_b(
            2,
            2,
            2,
            2,
            "b",
            "b",
            2,
            2,
            2,
            2
        );
        REQUIRE(test_a != test_b);
        test_a = test_b;
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that copy constructor works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        File test_b(test_a);
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that search works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        File test_b(
            2,
            2,
            2,
            2,
            "b",
            "b",
            2,
            2,
            2,
            2
        );
        std::vector<File> res;
        res = File::search();
        REQUIRE(res.size() == 2);
        res = File::search(
            "size = 2 AND "
            "mhash = 2 AND "
            "osdbhash = 2 AND "
            "added = 2 AND "
            "audios = 'b' AND "
            "subtitles = 'b' AND "
            "width = 2 AND "
            "height = 2 AND "
            "length = 2 AND "
            "movie_assigned_by = 2"
        );
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == test_b);
        res = File::search(
            "size = :num AND "
            "mhash = :num AND "
            "osdbhash = :num AND "
            "added = :num AND "
            "audios = :str AND "
            "subtitles = :str AND "
            "width = :num AND "
            "height = :num AND "
            "length = :num AND "
            "movie_assigned_by = :num"
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
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        test_a.set_size(2);
        test_a.set_mhash(2);
        test_a.set_osdbhash(2);
        test_a.set_added(2);
        test_a.set_audios("b");
        test_a.set_subtitles("b");
        test_a.set_width(2);
        test_a.set_height(2);
        test_a.set_length(2);
        test_a.set_movie_assigned_by(2);
		}
        auto res = File::search(
            "size = :num AND "
            "mhash = :num AND "
            "osdbhash = :num AND "
            "added = :num AND "
            "audios = :str AND "
            "subtitles = :str AND "
            "width = :num AND "
            "height = :num AND "
            "length = :num AND "
            "movie_assigned_by = :num"
            ,
            [](tntdb::Statement& st) {
                st.set("str", "b").set("num", 2);
            }
        );
        REQUIRE(res.size() == 1);
    }

    SECTION("Check that remove works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        File test_b(
            2,
            2,
            2,
            2,
            "b",
            "b",
            2,
            2,
            2,
            2
        );
        std::vector<File> res;
        res = File::search();
        REQUIRE(res.size() == 2);
        File::remove(
            "size = 2 AND "
            "mhash = 2 AND "
            "osdbhash = 2 AND "
            "added = 2 AND "
            "audios = 'b' AND "
            "subtitles = 'b' AND "
            "width = 2 AND "
            "height = 2 AND "
            "length = 2 AND "
            "movie_assigned_by = 2"
        );
        res = File::search();
        REQUIRE(res.size() == 1);
        File::remove();
        res = File::search();
        REQUIRE(res.size() == 0);
    }

    SECTION("Getting members works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        REQUIRE(test_a.get_paths().size() == 0);
    }

    SECTION("Serialization/deserialization works") {
        File test_a(
            1,
            1,
            1,
            1,
            "a",
            "a",
            1,
            1,
            1,
            1
        );
        cxxtools::SerializationInfo si;
        test_a.serialize(si);
        File test_b = File::deserialize(si);
        REQUIRE(test_a == test_b);
    }

    SECTION("Get by id works with empty strings") {
        File test_a(
            1,
            1,
            1,
            1,
            "",
            "",
            1,
            1,
            1,
            1
        );
        File test_b = File::get_by_id(test_a.get_db_id());
        REQUIRE(test_a == test_b);
    }

    unlink(db_url.c_str());
}

