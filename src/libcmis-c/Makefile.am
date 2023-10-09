
libcmis_c_@LIBCMIS_API_VERSION@_la_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/inc/libcmis-c \
	$(XML2_CFLAGS) \
	$(BOOST_CPPFLAGS)

libcmis_c_@LIBCMIS_API_VERSION@_la_CPPFLAGS = -DLIBCMIS_C_BUILD
if ENABLE_VISIBILITY
libcmis_c_@LIBCMIS_API_VERSION@_la_CXXFLAGS += -fvisibility=hidden
libcmis_c_@LIBCMIS_API_VERSION@_la_CPPFLAGS += -DLIBCMIS_C_VISIBILITY
endif

lib_LTLIBRARIES = libcmis-c-@LIBCMIS_API_VERSION@.la
libcmis_c_@LIBCMIS_API_VERSION@_la_SOURCES = \
	allowable-actions.cxx \
	document.cxx \
	error.cxx \
	folder.cxx \
	internals.hxx \
	oauth2-data.cxx \
	object-type.cxx \
	object.cxx \
	property-type.cxx \
	property.cxx \
	rendition.cxx \
	repository.cxx \
	session-factory.cxx \
	session.cxx \
	vectors.cxx

libcmis_c_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 6:0:0

libcmis_c_@LIBCMIS_API_VERSION@_la_LIBADD = \
	../libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)
