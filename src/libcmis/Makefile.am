AM_CXXFLAGS = \
	-I$(top_srcdir)/inc/libcmis \
	-I$(top_srcdir)/src/libcmis \
	$(XML2_CFLAGS) \
	$(CURL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-O0

noinst_LTLIBRARIES = libcmis.la

lib_LTLIBRARIES = libcmis-@LIBCMIS_API_VERSION@.la

libcmis_@LIBCMIS_API_VERSION@_la_SOURCES = \
	dummy.cxx

libcmis_la_SOURCES = \
	allowable-actions.cxx \
	atom-document.cxx \
	atom-document.hxx \
	atom-folder.cxx \
	atom-folder.hxx \
	atom-object-type.cxx \
	atom-object-type.hxx \
	atom-object.cxx \
	atom-object.hxx \
	atom-session.cxx \
	atom-session.hxx \
	atom-workspace.cxx \
	atom-workspace.hxx \
	base-session.cxx \
	base-session.hxx \
	document.cxx \
	folder.cxx \
	gdrive-allowable-actions.hxx \
	gdrive-document.cxx \
	gdrive-document.hxx \
	gdrive-folder.cxx \
	gdrive-folder.hxx \
	gdrive-object-type.cxx \
	gdrive-object-type.hxx \
	gdrive-object.cxx \
	gdrive-object.hxx \
	gdrive-property.cxx \
	gdrive-property.hxx \
	gdrive-repository.cxx \
	gdrive-repository.hxx \
	gdrive-session.cxx \
	gdrive-session.hxx \
	gdrive-utils.cxx \
	gdrive-utils.hxx \
	http-session.cxx \
	http-session.hxx \
	json-utils.cxx \
	json-utils.hxx \
	oauth2-data.cxx \
	oauth2-handler.cxx \
	oauth2-handler.hxx \
	oauth2-providers.cxx \
	oauth2-providers.hxx \
	object-type.cxx \
	object.cxx \
	onedrive-allowable-actions.hxx \
	onedrive-document.cxx \
	onedrive-document.hxx \
	onedrive-folder.cxx \
	onedrive-folder.hxx \
	onedrive-object-type.cxx \
	onedrive-object-type.hxx \
	onedrive-object.cxx \
	onedrive-object.hxx \
	onedrive-property.cxx \
	onedrive-property.hxx \
	onedrive-repository.cxx \
	onedrive-repository.hxx \
	onedrive-session.cxx \
	onedrive-session.hxx \
	onedrive-utils.cxx \
	onedrive-utils.hxx \
	property-type.cxx \
	property.cxx \
	rendition.cxx \
	repository.cxx \
	session-factory.cxx \
	sharepoint-allowable-actions.hxx \
	sharepoint-document.cxx \
	sharepoint-document.hxx \
	sharepoint-folder.cxx \
	sharepoint-folder.hxx \
	sharepoint-object-type.cxx \
	sharepoint-object-type.hxx \
	sharepoint-object.cxx \
	sharepoint-object.hxx \
	sharepoint-property.cxx \
	sharepoint-property.hxx \
	sharepoint-repository.cxx \
	sharepoint-repository.hxx \
	sharepoint-session.cxx \
	sharepoint-session.hxx \
	sharepoint-utils.cxx \
	sharepoint-utils.hxx \
	ws-document.cxx \
	ws-document.hxx \
	ws-folder.cxx \
	ws-folder.hxx \
	ws-navigationservice.cxx \
	ws-navigationservice.hxx \
	ws-object-type.cxx \
	ws-object-type.hxx \
	ws-object.cxx \
	ws-object.hxx \
	ws-objectservice.cxx \
	ws-objectservice.hxx \
	ws-relatedmultipart.cxx \
	ws-relatedmultipart.hxx \
	ws-repositoryservice.cxx \
	ws-repositoryservice.hxx \
	ws-requests.cxx \
	ws-requests.hxx \
	ws-session.cxx \
	ws-session.hxx \
	ws-soap.cxx \
	ws-soap.hxx \
	ws-versioningservice.cxx \
	ws-versioningservice.hxx \
	xml-utils.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -export-dynamic -no-undefined -version-info 5:0:0

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	libcmis.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LDFLAGS) \
	$(BOOST_DATE_TIME_LIBS)
