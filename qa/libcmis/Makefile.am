if !OS_WIN32
mockup_tests = \
	test-atom \
	test-factory \
	test-gdrive \
	test-onedrive \
	test-sharepoint \
	test-ws
endif

check_PROGRAMS = \
	test-json \
	test-utils \
	${mockup_tests}

check_LIBRARIES = \
	libtest.a

libtest_a_SOURCES = \
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx

if !OS_WIN32
libtest_a_SOURCES += \
	test-mockup-helpers.cxx \
	test-mockup-helpers.hxx
endif

libtest_a_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS)

test_utils_SOURCES =	\
	test-commons.cxx \
	test-decoder.cxx \
	test-soap.cxx \
	test-xmlutils.cxx

test_utils_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS)

test_utils_LDADD = \
	libtest.a \
	$(top_builddir)/src/libcmis/libcmis.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_atom_SOURCES =	\
	test-atom.cxx

test_atom_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_atom_LDADD = \
	libtest.a \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_gdrive_SOURCES =	\
	test-gdrive.cxx

test_gdrive_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_gdrive_LDADD = \
	libtest.a \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_onedrive_SOURCES =	\
	test-onedrive.cxx

test_onedrive_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_onedrive_LDADD = \
	libtest.a \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_sharepoint_SOURCES =	\
	test-sharepoint.cxx

test_sharepoint_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_sharepoint_LDADD = \
	libtest.a \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)


test_ws_SOURCES =	\
	test-ws.cxx

test_ws_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_ws_LDADD = \
	libtest.a \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_json_SOURCES =	\
	test-jsonutils.cxx

test_json_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_json_LDADD = \
	libtest.a \
	$(top_builddir)/src/libcmis/libcmis.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_DATE_TIME_LIBS)


test_factory_SOURCES =	\
	test-factory.cxx

test_factory_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_factory_LDADD = \
	libtest.a \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

TESTS = test-utils test-json ${mockup_tests}
