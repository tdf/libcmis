#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([libcmis_major_version], [0])
m4_define([libcmis_minor_version], [6])
m4_define([libcmis_micro_version], [2])
m4_define([libcmis_api_version], [libcmis_major_version.libcmis_minor_version])
m4_define([libcmis_version],[libcmis_api_version.libcmis_micro_version])


AC_PREREQ([2.63])
AC_INIT([libcmis], [libcmis_version])
AM_INIT_AUTOMAKE([1.10 foreign dist-xz])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
LT_INIT([win32-dll disable-static pic-only])
AC_CANONICAL_HOST

AX_CXX_COMPILE_STDCXX_11

# Check the options
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

AC_ARG_ENABLE([coverage],
    [AS_HELP_STRING([--enable-coverage], [Extract test coverage data])],
    [enable_coverage="$enableval"],
    [enable_coverage=no]
)
AM_CONDITIONAL(ENABLE_COVERAGE, test "x$enable_coverage" != "xno")

AS_IF([test "x$enable_coverage" != "xno"], [
       enable_tests=yes

       # Make sure we have gcc
       if test "x$GCC" != "xyes"; then
           AC_MSG_ERROR([GCC is required for --enable-coverage])
       fi

       AC_CHECK_PROG(LCOV, lcov, lcov)
       AC_CHECK_PROG(GENHTML, genhtml, genhtml)

       AS_IF([test "x$ac_cv_prog_LCOV" = "x"], [
           AC_MSG_ERROR([lcov is required for --enable-coverage])
       ])
       AS_IF([test "x$ac_cv_prog_GENHTML" = "x"], [
           AC_MSG_ERROR([genhtml is required for --enable-coverage])
       ])

       CFLAGS="-g -O0 --coverage"
       CXXFLAGS="-g -O0 --coverage"
       LDFLAGS+="--coverage"
])


LIBCMIS_API_VERSION=libcmis_api_version
AC_SUBST(LIBCMIS_API_VERSION)

AC_CONFIG_MACRO_DIR([m4])

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
	test -f $top_builddir/configure && break
done

SRCDIR=$srcdir
AC_SUBST(SRCDIR)

AC_LANG([C++])

# ==========================
# Platform check for windows
# ==========================
AC_MSG_CHECKING([for native Win32])
AS_CASE([$host], [*-*-mingw*], [native_win32=yes], [native_win32=no])
AC_MSG_RESULT([$native_win32])
AM_CONDITIONAL(OS_WIN32, [test "x$native_win32" = "xyes"])

# ====================
# Check for visibility
# ====================
AC_MSG_CHECKING([for -fvisibility=hidden compiler flag])
saved_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -fvisibility=hidden"
AC_TRY_COMPILE([], [], [have_visibility=yes], [have_visibility=no])
AC_MSG_RESULT([$have_visibility])
CPPFLAGS="$saved_CPPFLAGS"
AX_GCC_FUNC_ATTRIBUTE([visibility])
AM_CONDITIONAL([ENABLE_VISIBILITY], [
	test "x$have_visibility" = "xyes" && test "x$ax_cv_have_func_attribute_visibility" = "xyes"])

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
	AS_IF([test -z "$DOCBOOK2MAN"], [
		AC_PATH_PROGS([DOCBOOK2MAN], [docbook2x-man docbook-to-man docbook2man.pl docbook2man])
		AS_IF([test -z "$DOCBOOK2MAN"], [
			AC_MSG_ERROR([docbook-to-man is missing. Install docbook2X package.])
		])
	])
], [build_man=no])
AC_SUBST(DOCBOOK2MAN)
AM_CONDITIONAL([ENABLE_MAN], [test "x$build_man" != "xno"])

# ========
# Cppcheck
# ========
AC_PATH_PROG(CPPCHECK,[cppcheck],[])
AS_IF([test "x$ac_cv_path_CPPCHECK" != "x"], [enable_cppcheck=yes], [enable_cppcheck=no])
AC_SUBST(CPPCHECK)
AC_PATH_PROG([NPROC], [nproc], [])
AS_IF([test "x$NPROC" != "x"], [CPPCHECK_PARALLELISM=`$NPROC`], [CPPCHECK_PARALLELISM=1])
AC_SUBST([CPPCHECK_PARALLELISM])

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
	CFLAGS="$CFLAGS -Wall -pedantic -Wshadow -Wendif-labels -Wextra"
	CXXFLAGS="$CXXFLAGS -Wall -pedantic -Weffc++ -Wshadow -Wendif-labels -Wextra -Wsign-promo -Woverloaded-virtual -Wnon-virtual-dtor -Wsign-promo"
])

# Check for curl
PKG_CHECK_MODULES(CURL, [libcurl])
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)

# Check for lixml2
PKG_CHECK_MODULES(XML2, [libxml-2.0])
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

BOOST_REQUIRE([1.36])
BOOST_DATE_TIME
BOOST_SMART_PTR
BOOST_STRING_ALGO
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
	doc/cmis-client.xml
	inc/Makefile
	inc/libcmis-c/Makefile
	inc/libcmis/Makefile
	libcmis-c-$LIBCMIS_API_VERSION.pc:libcmis-c.pc.in
	libcmis-$LIBCMIS_API_VERSION.pc:libcmis.pc.in
	qa/Makefile
	qa/libcmis-c/Makefile
	qa/libcmis/Makefile
	qa/mockup/Makefile
	src/Makefile
	src/libcmis-c/Makefile
	src/libcmis/Makefile
])
AC_OUTPUT

AC_MSG_NOTICE([

libcmis $VERSION:
              Prefix: ${prefix}
            Compiler: ${CXX}
      Compiler flags: ${CXXFLAGS}
              client: ${enable_client}
              werror: ${enable_werror}
               tests: ${enable_tests}
       test coverage: ${enable_coverage}
                mans: ${build_man}
])
