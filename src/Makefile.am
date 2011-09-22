SUBDIRS = libcmis .

INCLUDES = -I$(top_srcdir)/include

check_PROGRAMS = test-atom

test_atom_SOURCES = test-atom.cxx
test_atom_LDADD = libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(DEPS_LIBS)
test_atom_CPPFLAGS = -I$(top_builddir)/lib/libcmis/libcmis.la

# Keep the following in mind when writing the tests:
#   * All test scripts are taking two arguments, REST_URL and SOAP_URL
#   * The test server is restarted after each test
TESTS_ENVIRONMENT = $(top_srcdir)/run-test.sh
TESTS = test-atom
