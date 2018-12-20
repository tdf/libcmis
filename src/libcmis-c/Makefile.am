
libcmis_c_@LIBCMIS_API_VERSION@_la_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/inc/libcmis-c \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS)

lib_LTLIBRARIES = libcmis-c-@LIBCMIS_API_VERSION@.la
libcmis_c_@LIBCMIS_API_VERSION@_la_SOURCES = \
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
	vectors.cxx \
	oauth2-data.cxx \
	rendition.cxx

libcmis_c_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 5:0:0

libcmis_c_@LIBCMIS_API_VERSION@_la_LIBADD = \
	../libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)
