lib_LTLIBRARIES = libcmis-mockup.la

libcmis_mockup_la_SOURCES = \
	mockup-config.cxx \
	curl-mockup.cxx \
	curl/curl.h \
	internals.hxx

libcmis_mockup_la_LIBADD = \
	$(top_builddir)/src/libcmis/libcmis.la \
	$(XML2_LIBS) \
	$(CURL_LIBS) \
	$(BOOST_SMART_PTR_LIBS) \
	$(BOOST_DATE_TIME_LIBS) \
	$(JSON_LIBS)

libcmis_mockup_la_LDFLAGS = -export-dynamic -no-undefined
