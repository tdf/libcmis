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

AC_ARG_WITH(hash-container-compat,
[
  --with-hash-container-compat use std::hash_foo instead of boost::unordered_foo
  ],
,with_hash_container_compat=no)

LIBCMIS_API_VERSION=libcmis_api_version
AC_SUBST(LIBCMIS_API_VERSION)

AC_CONFIG_MACRO_DIR([m4])

m4_pattern_allow([^BOOST_])

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
    test -f $top_builddir/configure && break
done


# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
# TODO Check the minimum versions
PKG_CHECK_MODULES(DEPS,
  [   libxml-2.0 >= 2.7
      libcurl >= 7.21
  ]
)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES(CPPUNIT, cppunit >= 1.12 )
AC_SUBST(CPPUNIT_CFLAGS)
AC_SUBST(CPPUNIT_LIBS)

AC_CONFIG_MACRO_DIR([m4])

m4_pattern_allow([^BOOST_])
BOOST_SMART_PTR
BOOST_PROGRAM_OPTIONS

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
