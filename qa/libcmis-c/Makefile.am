
if ENABLE_TESTS
check_PROGRAMS = \
	test-api \
	test-c-build

test_api_SOURCES = \
	test-api.cxx \
	test-allowable-actions.cxx \
	test-document.cxx \
	test-dummies.hxx \
	test-dummies.cxx \
	test-folder.cxx \
	test-object.cxx \
	test-object-type.cxx \
	test-property.cxx \
	test-property-type.cxx \
	test-repository.cxx

test_api_CXXFLAGS = -I$(top_srcdir)/src/ -I$(top_srcdir)/src/libcmis-c/ $(XML2_CFLAGS) $(BOOST_CPPFLAGS)

test_api_LDADD = \
	$(top_builddir)/src/libcmis-c/libcmis-c-@LIBCMIS_API_VERSION@.la \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

test_c_build_SOURCES = \
	test-build.c

test_c_build_CFLAGS = -I$(top_srcdir)/src/ -I$(top_srcdir)/src/libcmis-c/ $(XML2_CFLAGS)

test_c_build_LDADD = \
	$(top_builddir)/src/libcmis-c/libcmis-c-@LIBCMIS_API_VERSION@.la \
	$(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

TESTS = test-api

endif
