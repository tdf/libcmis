lib_LTLIBRARIES = libcmis-mockup.la

libcmis_mockup_la_SOURCES = \
	curl-mockup.cxx \
	curl/curl.h

libcmis_mockup_la_LIBADD = \
	$(top_builddir)/src/libcmis/libcmis.la

libcmis_mockup_la_LDFLAGS = -export-dynamic -no-undefined
