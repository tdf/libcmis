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
	test-nosrv-mockup \
	test-atom \
	test-ws

test_nosrv_utils_SOURCES =	\
	test-decoder.cxx \
	test-soap.cxx \
	test-xmlutils.cxx \
	test-helpers.hxx \
	test-helpers.cxx \
	test-main.cxx

test_nosrv_utils_CPPFLAGS = \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-O0

test_nosrv_utils_LDADD = \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_nosrv_mockup_SOURCES =	\
	test-mock-atom.cxx \
	test-main.cxx

test_nosrv_mockup_CPPFLAGS = \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_builddir)/qa/libcmis-mockup \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-O0

test_nosrv_mockup_LDADD = \
	$(top_builddir)/qa/libcmis-mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_atom_SOURCES =	\
	test-atom.cxx \
	test-helpers.hxx \
	test-helpers.cxx \
	test-main.cxx

test_atom_CPPFLAGS = \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-O0

test_atom_LDADD = \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_ws_SOURCES =	\
	test-ws.cxx \
	test-helpers.hxx \
	test-helpers.cxx \
	test-main.cxx

test_ws_CPPFLAGS = \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-O0

test_ws_LDADD = \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
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
TESTS = test-nosrv-utils test-nosrv-mockup ${long_tests}

endif
