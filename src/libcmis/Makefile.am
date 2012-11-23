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

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 3:0:0

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS)

libcmis_@LIBCMIS_API_VERSION@_la_includedir = $(includedir)/libcmis-@LIBCMIS_API_VERSION@/libcmis
