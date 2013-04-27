
libcmis_c_@LIBCMIS_API_VERSION@_la_CXXFLAGS = -I$(top_srcdir)/src/ $(XML2_CFLAGS) $(BOOST_CPPFLAGS)

libcmis_c_@LIBCMIS_API_VERSION@_la_include_HEADERS = \
	allowable-actions.h \
	document.h \
	error.h \
	folder.h \
	object.h \
	object-type.h \
	property.h \
	property-type.h \
	repository.h \
	types.h \
	session.h \
	session-factory.h \
	vectors.h

lib_LTLIBRARIES = libcmis-c-@LIBCMIS_API_VERSION@.la
libcmis_c_@LIBCMIS_API_VERSION@_la_SOURCES = \
	$(libcmis_c_headers_public) \
	allowable-actions.cxx \
	document.cxx \
	error.cxx \
	folder.cxx \
	internals.hxx \
	object.cxx \
	object-type.cxx \
	property.cxx \
	property-type.cxx \
	repository.cxx \
	session.cxx \
	session-factory.cxx \
	vectors.cxx

libcmis_c_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 4:0:0

libcmis_c_@LIBCMIS_API_VERSION@_la_LIBADD = \
	../libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

libcmis_c_@LIBCMIS_API_VERSION@_la_includedir = $(includedir)/libcmis-c-@LIBCMIS_API_VERSION@/libcmis-c
