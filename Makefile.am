ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/ext -I$(top_builddir)/src

lib_LTLIBRARIES = libtomovee.la

libtomovee_la_LDFLAGS = -version-info 0:1:0
libtomovee_la_SOURCES = src/helper.cpp src/structures.cpp

src/helper.cpp: src/sql_init.h

bin_PROGRAMS = tomovee_miner

tomovee_miner_SOURCES = src/miner.cpp
tomovee_miner_LDADD = libtomovee.la $(SQLITE_LIBS)

src/sql_init.h: $(top_srcdir)/src/structures.sql
	echo "#define SQL_INIT \"`cat "$<"`\"" | sed 's|$$|\\|' | sed '$$ s|"\\|"|' > $@

if TESTS
AM_CXXFLAGS  += -I$(top_srcdir)/tests
noinst_PROGRAMS  = tomovee_tests

tomovee_tests_SOURCES = tests/main_test.cpp tests/helper_test.cpp
tomovee_tests_LDADD = libtomovee.la $(SQLITE_LIBS)


test: tomovee_tests
	@cd $(top_srcdir); $(top_builddir)/tomovee_tests -s
.PHONY: test

endif
