#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([libcmis_major_version], [0])
m4_define([libcmis_minor_version], [3])
m4_define([libcmis_micro_version], [0])
m4_define([libcmis_api_version], [libcmis_major_version.libcmis_minor_version])
m4_define([libcmis_version],[libcmis_api_version.libcmis_micro_version])


AC_PREREQ([2.63])
AC_INIT([libcmis], [libcmis_version])
AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ARG_ENABLE(client,
	AC_HELP_STRING([--disable-client],[Compile only the library, not the client tool]),
	enable_client=$enableval, enable_client=yes)
AM_CONDITIONAL(ENABLE_CLIENT, test "x$enable_client" != "xno")

AC_ARG_ENABLE([tests],
	[AS_HELP_STRING([--disable-tests], [Do not build the unit tests])],
	[enable_tests="$enableval"],
	[enable_tests=yes]
)
AM_CONDITIONAL(ENABLE_TESTS, test "x$enable_tests" != "xno")

AS_IF([test "x$enable_tests" != "xno"], [
	AC_ARG_ENABLE([long-tests],
		[AS_HELP_STRING([--disable-long-tests], [Do not build the unit tests needing to start a server])],
		[enable_long_tests="$enableval"],
		[enable_long_tests=yes]
	)
], [enable_long_tests=no])
AM_CONDITIONAL(ENABLE_LONG_TESTS, test "x$enable_long_tests" != "xno")

LIBCMIS_API_VERSION=libcmis_api_version
AC_SUBST(LIBCMIS_API_VERSION)

AC_CONFIG_MACRO_DIR([m4])

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
	test -f $top_builddir/configure && break
done

AC_LANG([C++])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL

# TODO Check for docbook-to-man
# =============
# Documentation
# =============
AC_ARG_WITH(man,
	[AS_HELP_STRING([--without-man], [Do not build manpage])],
	[with_man="$withval"],
	[with_man=yes]
)
AS_IF([test "x$with_man" != "xno"], [
	build_man=yes
	AC_PATH_PROG(DOCBOOK2X,[docbook-to-man])
	AS_IF([test -z "$DOCBOOK2X"], [
		AC_PATH_PROG(DOCBOOK2X,[docbook2man.pl])
		AS_IF([test -z "$DOCBOOK2X"], [
			AC_MSG_ERROR([docbook-to-man is missing. Install docbook2X package.])
		])
	])
], [build_man=no])
AC_SUBST(DOCBOOK2X)
AM_CONDITIONAL([WITH_LIBCMIS_MAN], [test "x$build_man" != "xno"])

AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--disable-werror], [Treat all warnings as errors, usefull for development])],
	[enable_werror="$enableval"],
	[enable_werror=yes]
)
AS_IF([test x"$enable_werror" != "xno"], [
	CFLAGS="$CFLAGS -Werror"
	CXXFLAGS="$CXXFLAGS -Werror"
])
AS_IF([test x"$GCC" = xyes], [
	# Be tough with warnings and produce less careless code
	CFLAGS="$CFLAGS -Wall -pedantic"
	CXXFLAGS="$CXXFLAGS -Wall -pedantic -Weffc++"
])

# Check for curl
PKG_CHECK_MODULES(CURL,[
	libcurl
])
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)

# Check for lixml2
PKG_CHECK_MODULES(XML2,[
	libxml-2.0
])
AC_SUBST(XML2_CFLAGS)
AC_SUBST(XML2_LIBS)

# Check for cppunit
AS_IF([test "x$enable_tests" != "xno"], [
	PKG_CHECK_MODULES(CPPUNIT, cppunit >= 1.12 )
	AC_SUBST(CPPUNIT_CFLAGS)
	AC_SUBST(CPPUNIT_LIBS)
])

# Check for boost
m4_pattern_allow([^BOOST_])

BOOST_DATE_TIME
BOOST_SMART_PTR
BOOST_UUID

AS_IF([test "x$enable_client" != "xno"], [
	BOOST_PROGRAM_OPTIONS
])
AC_SUBST(BOOST_CPPFLAGS)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([
	Makefile
	libcmis.pc
	libcmis-c.pc
    run-test.sh
	src/Makefile
	src/libcmis/Makefile
	src/libcmis-c/Makefile
])
AC_OUTPUT

AC_MSG_NOTICE([

libcmis $VERSION:
              Prefix: ${prefix}
      Compiler flags: ${CXXFLAGS}
              client: ${enable_client}
              werror: ${enable_werror}
               tests: ${enable_tests}
          long tests: ${enable_long_tests}
                mans: ${build_man}
])
