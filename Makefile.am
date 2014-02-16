ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -I$(top_srcdir)/src

lib_LTLIBRARIES = libtomovee.la

libtomovee_la_LDFLAGS = -version-info 0:1:0
libtomovee_la_SOURCES = src/helper.cpp

if TESTS
AM_CXXFLAGS  += -I$(top_srcdir)/tests
noinst_PROGRAMS  = tomovee_tests

tomovee_tests_SOURCES = tests/main.cpp
tomovee_tests_LDADD = -ltomovee


test: tomovee_tests
	@cd $(top_srcdir); $(top_builddir)/tomovee_tests
.PHONY: test

all-local: test

endif
