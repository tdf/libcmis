
libcmis_c_@LIBCMIS_API_VERSION@_la_CXXFLAGS = -I$(top_srcdir)/src/ $(XML2_CFLAGS) $(BOOST_CPPFLAGS)

libcmis_c_@LIBCMIS_API_VERSION@_la_include_HEADERS = \
	allowable-actions.h \
	error.h \
	folder.h \
	object.h \
	object-type.h \
	property.h \
	property-type.h \
	repository.h \
	session.h \
	session-factory.h

lib_LTLIBRARIES = libcmis_c-@LIBCMIS_API_VERSION@.la
libcmis_c_@LIBCMIS_API_VERSION@_la_SOURCES = \
	$(libcmis_c_headers_public) \
	allowable-actions.cxx \
	error.cxx \
	folder.cxx \
	internals.hxx \
	object.cxx \
	object-type.cxx \
	property.cxx \
	property-type.cxx \
	repository.cxx \
	session.cxx \
	session-factory.cxx

libcmis_c_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 2:2:0

libcmis_c_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

libcmis_c_@LIBCMIS_API_VERSION@_la_includedir = $(includedir)/libcmis_c-@LIBCMIS_API_VERSION@/libcmis-c

if ENABLE_TESTS
check_PROGRAMS = \
	test-api

test_api_SOURCES = \
	test-api.cxx \
	test-dummies.hxx \
	test-dummies.cxx \
	test-repository.cxx

test_api_CXXFLAGS = -I$(top_srcdir)/src/ $(XML2_CFLAGS) $(BOOST_CPPFLAGS)

test_api_LDADD = \
	libcmis_c-@LIBCMIS_API_VERSION@.la \
	$(top_srcdir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(CPPUNIT_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

TESTS = test-api

endif
