SUBDIRS = .

INCLUDES = -I$(top_srcdir)/include

libcmis_headers_public = \
    document.hxx \
	folder.hxx \
	cmis-object.hxx \
	session.hxx \
	session-factory.hxx 

lib_LTLIBRARIES = libcmis-@LIBCMIS_API_VERSION@.la
libcmis_@LIBCMIS_API_VERSION@_la_SOURCES = \
	$(libcmis_headers_public) \
	atom-document.hxx \
	atom-folder.hxx \
	atom-object.hxx \
	atom-session.hxx \
	session-factory.cxx \
	atom-document.cxx \
	atom-folder.cxx \
	atom-object.cxx \
	atom-session.cxx \
	atom-utils.hxx \
	atom-utils.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -no-undefined

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(DEPS_LIBS) \
	$(BOOST_SMART_PTR)

libcmisdir = $(includedir)/libcmis
libcmis_HEADERS = $(libcmis_headers_public)

check_PROGRAMS = test-atom

test_atom_SOURCES =	test-atom.cxx

test_atom_LDADD = libcmis-@LIBCMIS_API_VERSION@.la \
	$(DEPS_LIBS) \
	$(CPPUNIT_LIBS)

TESTS_ENVIRONMENT = $(top_srcdir)/run-test.sh
TESTS = test-atom

EXTRA_DIST = 
