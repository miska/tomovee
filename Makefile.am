ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/ext -I$(top_srcdir)/ext/catch -I$(top_builddir)/src -I$(top_srcdir)/ext/AhoCorasik

lib_LTLIBRARIES = libtomovee.la

libtomovee_la_LDFLAGS = -version-info 0:1:0
libtomovee_la_SOURCES = src/helper.cpp src/structures.cpp src/path.cpp \
                        src/file.cpp src/movie.cpp src/lang_codes.cpp

bin_PROGRAMS = tomovee_miner tomovee_cli

tomovee_miner_SOURCES = src/miner.cpp
tomovee_miner_LDADD = libtomovee.la

tomovee_cli_SOURCES = src/cli.cpp
tomovee_cli_LDADD = libtomovee.la

ISO-639-2_utf-8.txt:
	cd $(top_builddir); wget http://loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt
	echo -e "\nempty|||Empty|" >> $@
	sed -i '1 s/\xEF\xBB\xBF//' $@

src/lang_codes.re: ISO-639-2_utf-8.txt generate_lang_codes.sh
	cd $(top_builddir); $(abs_top_srcdir)/generate_lang_codes.sh

src/lang_codes.cpp:
	re2c --case-insensitive -bi -o $@ $<

if TESTS
AM_CXXFLAGS  += -I$(top_srcdir)/tests
noinst_PROGRAMS  = tomovee_tests

tomovee_tests_SOURCES = tests/main_test.cpp tests/helper_test.cpp \
                        src/path_test.cpp src/file_test.cpp \
			src/movie_test.cpp tests/lang_codes_test.cpp
tomovee_tests_LDADD = libtomovee.la $(SQLITE_LIBS)

test: tomovee_tests
	@cd $(top_srcdir); $(abs_top_builddir)/tomovee_tests -s
.PHONY: test

endif
