AC_INIT([tomovee], [0.1], [michal@hrusecky.net])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz subdir-objects])
AM_SILENT_RULES([yes])

AC_CONFIG_FILES([Makefile Doxyfile])
AC_CONFIG_HEADERS([src/config.h])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S

m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
AX_CXX_COMPILE_STDCXX_11([],[mandatory])

LT_INIT([disable-static pic-only])

AC_ARG_ENABLE([werror],
        [AS_HELP_STRING([--enable-werror],
            [Treat all warnings as errors])],
        [enable_werror="$enableval"],
        [enable_werror=no]
)
AS_IF([test x"$enable_werror" != "xno"], [
        CXXFLAGS="$CXXFLAGS -Werror"
])
AC_ARG_ENABLE([debug],
        [AS_HELP_STRING([--enable-debug],
            [Enable debugging])],
        [enable_debug="$enableval"],
        [enable_debug=no]
)
AS_IF([test x"$enable_debug" != "xno"], [
        CXXFLAGS="$CXXFLAGS -g -O0"
])
AC_ARG_ENABLE([gcov],
        [AS_HELP_STRING([--enable-gcov],
            [Enable gcov])],
        [enable_gcov="$enableval"],
        [enable_gcov=no]
)
AS_IF([test x"$enable_gcov" != "xno"], [
        CXXFLAGS="$CXXFLAGS -O0 -fprofile-arcs -ftest-coverage"
        LDFLAGS="$LDFLAGS -lgcov"
])
AC_ARG_ENABLE([tests],
        [AS_HELP_STRING([--enable-tests],
            [Compile and run test suite])],
        [enable_tests="$enableval"],
        [enable_tests=no]
)
AS_IF([test x"$enable_tests" != "xno"], [
        AM_CONDITIONAL(TESTS, true)
],[
        AM_CONDITIONAL(TESTS, false)
])

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([tntdb/connection.h], [],
                 AC_MSG_ERROR([cannot find required header tntdb/connection.h]))
AX_CXX_CHECK_LIB(tntdb, tntdb::dropCached(unsigned keep = 0), [],
                AC_MSG_ERROR([cannot find required tntdb library]))
AC_CHECK_HEADERS([cxxtools/jsonserializer.h], [],
                 AC_MSG_ERROR([cannot find required header cxxtools/jsonserializer.h]))
AC_CHECK_LIB(cxxtools, [main], [],
                AC_MSG_ERROR([cannot find required cxxtools library]))
AC_LANG_POP([C++])

AC_CHECK_HEADERS([magic.h], [],
                 AC_MSG_ERROR([cannot find required header magic.h]))
AC_CHECK_LIB(magic, magic_open, [],
                AC_MSG_ERROR([cannot find required magic library]))

CXXFLAGS="$CXXFLAGS -Wall -Wextra -pedantic"

AC_OUTPUT

AC_MSG_NOTICE([
==========================================
Build configuration:
        werror:          ${enable_werror}
        debug:           ${enable_debug}
        gcov:            ${enable_gcov}
        tests:           ${enable_tests}
==========================================
])
