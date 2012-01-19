AM_CXXFLAGS = -I$(top_srcdir)/src/libcmis $(XML2_CFLAGS) $(CURL_CFLAGS) $(BOOST_CPPFLAGS)

libcmis_@LIBCMIS_API_VERSION@_la_include_HEADERS = \
	document.hxx \
	folder.hxx \
	object.hxx \
	session.hxx \
	session-factory.hxx \
    allowable-actions.hxx	

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
	atom-utils.cxx \
	atom-workspace.hxx \
	atom-workspace.cxx \
	atom-allowable-actions.hxx \
	atom-allowable-actions.cxx \
	allowable-actions.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

libcmis_@LIBCMIS_API_VERSION@_la_includedir = $(includedir)/libcmis-@LIBCMIS_API_VERSION@/libcmis

if ENABLE_TESTS
check_PROGRAMS = test-atom

test_atom_SOURCES =	\
	test-atom.cxx \
	test-decoder.cxx

test_atom_LDADD = libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

TESTS_ENVIRONMENT = $(top_srcdir)/run-test.sh
TESTS = test-atom
endif
