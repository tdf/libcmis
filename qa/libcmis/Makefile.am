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

test_utils_SOURCES =	\
	test-commons.cxx \
	test-decoder.cxx \
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx \
	test-soap.cxx \
	test-xmlutils.cxx

test_utils_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS)

test_utils_LDADD = \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_atom_SOURCES =	\
	test-atom.cxx \
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx

test_atom_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_atom_LDADD = \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_gdrive_SOURCES =	\
	test-gdrive.cxx \
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx

test_gdrive_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_gdrive_LDADD = \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_onedrive_SOURCES =	\
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx \
	test-onedrive.cxx

test_onedrive_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_onedrive_LDADD = \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_sharepoint_SOURCES =	\
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx \
	test-sharepoint.cxx

test_sharepoint_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_sharepoint_LDADD = \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)


test_ws_SOURCES =	\
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx \
	test-mockup-helpers.cxx \
	test-mockup-helpers.hxx \
	test-ws.cxx

test_ws_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_ws_LDADD = \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_json_SOURCES =	\
	test-helpers.cxx \
	test-helpers.hxx \
	test-jsonutils.cxx \
	test-main.cxx

test_json_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_json_LDADD = \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)


test_factory_SOURCES =	\
	test-factory.cxx \
	test-helpers.cxx \
	test-helpers.hxx \
	test-main.cxx \
	test-mockup-helpers.cxx \
	test-mockup-helpers.hxx

test_factory_CPPFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	-I$(top_srcdir)/qa/mockup \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DDATA_DIR=\"$(top_srcdir)/qa/libcmis/data\"

test_factory_LDADD = \
	$(top_builddir)/qa/mockup/libcmis-mockup.la \
	$(XML2_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

TESTS = test-utils test-json ${mockup_tests}
