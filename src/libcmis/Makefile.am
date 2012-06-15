AM_CXXFLAGS = -I$(top_srcdir)/src/libcmis $(XML2_CFLAGS) $(CURL_CFLAGS) $(BOOST_CPPFLAGS)

libcmis_@LIBCMIS_API_VERSION@_la_include_HEADERS = \
    allowable-actions.hxx \
	document.hxx \
	exception.hxx \
	folder.hxx \
	object-type.hxx \
	object.hxx \
	property-type.hxx \
	property.hxx \
	repository.hxx \
	session-factory.hxx \
	session.hxx \
	xml-utils.hxx \
	xmlserializable.hxx

lib_LTLIBRARIES = libcmis-@LIBCMIS_API_VERSION@.la
libcmis_@LIBCMIS_API_VERSION@_la_SOURCES = \
	$(libcmis_headers_public) \
	atom-document.hxx \
	atom-folder.hxx \
	atom-object.hxx \
	atom-object-type.hxx \
	atom-object-type.cxx \
	atom-session.hxx \
	session-factory.cxx \
	atom-document.cxx \
	atom-folder.cxx \
	atom-object.cxx \
	atom-session.cxx \
	atom-workspace.hxx \
	atom-workspace.cxx \
	atom-allowable-actions.hxx \
	atom-allowable-actions.cxx \
	base-session.hxx \
	base-session.cxx \
	allowable-actions.cxx \
	property.cxx \
	property-type.cxx \
	xml-utils.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 2:2:0

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
	test-decoder.cxx \
	test-xmlutils.cxx \
	test-helpers.hxx \
	test-helpers.cxx

test_atom_LDADD = libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

TESTS_ENVIRONMENT = $(top_srcdir)/run-test.sh
TESTS = test-atom
endif
