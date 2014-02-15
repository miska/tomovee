ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -I$(top_srcdir)/src

if TESTS
AM_CXXFLAGS  += -I$(top_srcdir)/tests
noinst_PROGRAMS  = tomovee_tests

tomovee_tests_SOURCES = tests/main.cpp src/helper.cpp


test: tomovee_tests
	@cd $(top_srcdir); $(top_builddir)/tomovee_tests
.PHONY: test

all-local: test

endif
