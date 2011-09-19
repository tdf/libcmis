INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libcmis-@LIBCMIS_API_VERSION@.la
libcmis_@LIBCMIS_API_VERSION@_la_SOURCES = \
	session.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -no-undefined

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(LIBXML_LIBS) \
	$(LIBCURL_LIBS)

EXTRA_DIST = 
