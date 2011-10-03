#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([libcmis_major_version], [0])
m4_define([libcmis_minor_version], [1])
m4_define([libcmis_micro_version], [0])
m4_define([libcmis_version],
                    [libcmis_major_version.libcmis_minor_version.libcmis_micro_version])
m4_define([libcmis_minor_api_version], [m4_eval(libcmis_minor_version + libcmis_minor_version%2)])
m4_define([libcmis_api_version],
                    [libcmis_major_version.libcmis_minor_api_version])


AC_PREREQ([2.63])
AC_INIT([libcmis], [libcmis_version])
AM_INIT_AUTOMAKE

AC_ARG_ENABLE(client,
    AC_HELP_STRING([--disable-client],[Compile only the library, not the client tool]),
    enable_client=$enableval, enable_client=yes)

LIBCMIS_API_VERSION=libcmis_api_version
AC_SUBST(LIBCMIS_API_VERSION)

AC_CONFIG_MACRO_DIR([m4])

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
    test -f $top_builddir/configure && break
done


# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Check for curl
LIBCURL_CHECK_CONFIG
AC_SUBST(CURL_CFLAGS)
AC_SUBST(LIBCURL)

# Check for lixml2
AC_PATH_PROGS([TOOL_XML2_CONFIG], [xml2-config])
AS_IF([test -n "$TOOL_XML2_CONFIG"],
      [XML2_CFLAGS=`$TOOL_XML2_CONFIG --cflags`
       XML2_LIBS=`$TOOL_XML2_CONFIG --libs`],
      [AC_MSG_ERROR([missing program 'xml2-config'; is 'libxml2' or
'libxml2-devel' installed?])])
AC_SUBST([XML2_CFLAGS])
AC_SUBST([XML2_LIBS]) 

# Check for cppunit
PKG_CHECK_MODULES(CPPUNIT, cppunit >= 1.12 )
AC_SUBST(CPPUNIT_CFLAGS)
AC_SUBST(CPPUNIT_LIBS)

# Check for boost
m4_pattern_allow([^BOOST_])

BOOST_SMART_PTR
BOOST_PROGRAM_OPTIONS

if test "x$boost_cv_lib_program_options" != "xyes"; then
    enable_client="no";
fi
AM_CONDITIONAL(ENABLE_CLIENT, test "x$enable_client" = "xyes")

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 libcmis.pc
                 src/Makefile
                 src/libcmis/Makefile])
AC_OUTPUT

echo ""
echo "libcmis $VERSION"
echo ""
echo "              Prefix: ${prefix}"
echo "      Compiler flags: ${CPPFLAGS}"
echo "              Client: ${enable_client}"
echo ""
