#!/bin/sh
cat > src/lang_codes.re << EOF
#include <string>
#include <stdio.h>
#include "lang_codes.h"

int lang2code(c_str &p, std::string& ret) {
    const unsigned char* marker = p;
/*!re2c
    re2c:define:YYCTYPE  = "unsigned char";
    re2c:define:YYCURSOR = p;
    re2c:define:YYMARKER = marker;
    re2c:yyfill:enable = 0;
	"\000" { return 0; }
	"\n" { return 0; }
EOF
cat ISO-639-2_utf-8.txt | \
sed 's/^\(.*\)|.*|.*|\(.*\)|.*/\t"\2" { if(!ret.empty()) ret+=","; ret += "\1"; return 1; }/' >> src/lang_codes.re
cat >> src/lang_codes.re << EOF
	. { return 1; }
*/
}
EOF

cat > tests/lang_codes_test.cpp << EOF
#include "catch/catch.hpp"
#include "lang_codes.h"

TEST_CASE( "Lang2Code works", "[helper][lang2code]" ) {
    SECTION("Getting it works") {
        const char* test = 
        `cat ISO-639-2_utf-8.txt | \
        sed 's/^\(.*\)|.*|.*|\(.*\)|.*/"blabla \2 bla "/'`;
        std::string ref_ret = 
        `cat ISO-639-2_utf-8.txt | \
        sed 's/^\(.*\)|.*|.*|\(.*\)|.*/"\1,"/'`;
        ref_ret.pop_back();
        std::string ret;
        c_str p = (c_str)test;
        while(lang2code(p, ret));
        REQUIRE(ret == ref_ret);
    }
}
EOF
