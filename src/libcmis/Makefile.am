AM_CXXFLAGS = -I$(top_srcdir)/src/libcmis $(XML2_CFLAGS) $(CURL_CFLAGS) $(BOOST_CPPFLAGS) -O0

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
	session-factory.cxx \
	base-session.hxx \
	base-session.cxx \
	allowable-actions.cxx \
	document.cxx \
	folder.cxx \
	object.cxx \
	object-type.cxx \
	property.cxx \
	property-type.cxx \
	repository.cxx \
	xml-utils.cxx \
	atom-document.hxx \
	atom-folder.hxx \
	atom-object.hxx \
	atom-object-type.hxx \
	atom-object-type.cxx \
	atom-session.hxx \
	atom-document.cxx \
	atom-folder.cxx \
	atom-object.cxx \
	atom-session.cxx \
	atom-workspace.hxx \
	atom-workspace.cxx \
	ws-document.hxx \
	ws-document.cxx \
	ws-folder.hxx \
	ws-folder.cxx \
	ws-navigationservice.hxx \
	ws-navigationservice.cxx \
	ws-object.hxx \
	ws-object.cxx \
	ws-object-type.hxx \
	ws-object-type.cxx \
	ws-objectservice.hxx \
	ws-objectservice.cxx \
	ws-relatedmultipart.hxx \
	ws-relatedmultipart.cxx \
	ws-repositoryservice.hxx \
	ws-repositoryservice.cxx \
	ws-requests.hxx \
	ws-requests.cxx \
	ws-soap.hxx \
	ws-soap.cxx \
	ws-session.hxx \
	ws-session.cxx \
	ws-versioningservice.hxx \
	ws-versioningservice.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 3:1:0

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

libcmis_@LIBCMIS_API_VERSION@_la_includedir = $(includedir)/libcmis-@LIBCMIS_API_VERSION@/libcmis

if ENABLE_TESTS
#
# Note that the test names are important here due to the environment test.
# Some tests will need to have a running CMIS server while some others don't.
#
# In order to allow running the tests that don't need the server during the
# packaging process, a simple naming convention wil distinguish between those
# two kinds of tests:
#
# 	test-nosrv-* is a test that doesn't need a running server,
# 	others will trigger a server start / stop
#
check_PROGRAMS = \
	test-nosrv-utils \
	test-atom \
	test-ws

test_nosrv_utils_SOURCES =	\
	test-decoder.cxx \
	test-soap.cxx \
	test-xmlutils.cxx \
	test-helpers.hxx \
	test-helpers.cxx \
	test-main.cxx

test_nosrv_utils_LDADD = libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_atom_SOURCES =	\
	test-atom.cxx \
	test-helpers.hxx \
	test-helpers.cxx \
	test-main.cxx

test_atom_LDADD = libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_ws_SOURCES =	\
	test-ws.cxx \
	test-helpers.hxx \
	test-helpers.cxx \
	test-main.cxx

test_ws_LDADD = libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

if ENABLE_LONG_TESTS
long_tests = \
	test-atom \
	test-ws
endif

TESTS_ENVIRONMENT = $(SHELL) $(top_builddir)/run-test.sh
TESTS = test-nosrv-utils ${long_tests}

endif
