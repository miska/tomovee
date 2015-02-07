/**
 * file path_test.cpp
 *
 * brief Tests for path
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

#include <string>
#include <stdio.h>
#include <unistd.h>
#include "catch.hpp"
#include "path.hpp"

extern std::string db_url;

TEST_CASE( "Path works", "[path]" ) {
    db_url  = "sqlite:";
    db_url += tmpnam(NULL);
    unlink(db_url.c_str());
    Path::db_reinit();

    SECTION("Check that getters works") {
        Path test_a(
            "a",
            "a",
            1
        );
        REQUIRE(test_a.get_db_id() != 0);
    }

    SECTION("Check that getters works") {
        Path test_a(
            "a",
            "a",
            1
        );
        REQUIRE(test_a.get_storage() == "a");
        REQUIRE(test_a.get_path() == "a");
        REQUIRE(test_a.get_checked() == 1);
    }
    SECTION("Check that setters works") {
        Path test_a(
            "a",
            "a",
            1
        );
        test_a.set_storage("b");
        test_a.set_path("b");
        test_a.set_checked(2);
        REQUIRE(test_a.get_storage() == "b");
        REQUIRE(test_a.get_path() == "b");
        REQUIRE(test_a.get_checked() == 2);
    }

    SECTION("Check that comparison works") {
        Path test_a(
            "a",
            "a",
            1
        );
        Path test_b(
            "b",
            "b",
            2
        );
        REQUIRE(test_a != test_b);
        test_a.set_storage("b");
        test_a.set_path("b");
        test_a.set_checked(2);
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that assignment works") {
        Path test_a(
            "a",
            "a",
            1
        );
        Path test_b(
            "b",
            "b",
            2
        );
        REQUIRE(test_a != test_b);
        test_a = test_b;
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that copy constructor works") {
        Path test_a(
            "a",
            "a",
            1
        );
        Path test_b(test_a);
        REQUIRE(test_a == test_b);
    }

    SECTION("Check that search works") {
        Path test_a(
            "a",
            "a",
            1
        );
        Path test_b(
            "b",
            "b",
            2
        );
        std::vector<Path> res;
        res = Path::search();
        REQUIRE(res.size() == 2);
        res = Path::search(
            "storage = 'b' AND "
            "path = 'b' AND "
            "checked = 2"
        );
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == test_b);
        res = Path::search(
            "storage = :str AND "
            "path = :str AND "
            "checked = :num"
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
        Path test_a(
            "a",
            "a",
            1
        );
        test_a.set_storage("b");
        test_a.set_path("b");
        test_a.set_checked(2);
        }
        auto res = Path::search(
            "storage = :str AND "
            "path = :str AND "
            "checked = :num"
            ,
            [](tntdb::Statement& st) {
                st.set("str", "b").set("num", 2);
            }
        );
        REQUIRE(res.size() == 1);
    }

    SECTION("Check that remove works") {
        Path test_a(
            "a",
            "a",
            1
        );
        Path test_b(
            "b",
            "b",
            2
        );
        std::vector<Path> res;
        res = Path::search();
        REQUIRE(res.size() == 2);
        Path::remove(
            "storage = 'b' AND "
            "path = 'b' AND "
            "checked = 2"
        );
        res = Path::search();
        REQUIRE(res.size() == 1);
        Path::remove();
        res = Path::search();
        REQUIRE(res.size() == 0);
    }

    SECTION("Getting members works") {
        Path test_a(
            "a",
            "a",
            1
        );
    }

    SECTION("Serialization/deserialization works") {
        Path test_a(
            "a",
            "a",
            1
        );
        cxxtools::SerializationInfo si;
        test_a.serialize(si);
        Path test_b = Path::deserialize(si);
        REQUIRE(test_a == test_b);
    }

    SECTION("Get by id works with empty strings") {
        Path test_a(
            "",
            "",
            1
        );
        Path test_b = Path::get_by_id(test_a.get_db_id());
        REQUIRE(test_a == test_b);
    }

    unlink(db_url.c_str());
}

