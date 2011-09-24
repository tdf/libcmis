SUBDIRS = .

INCLUDES = -I$(top_srcdir)/include

libcmis_headers_public = \
	folder.hxx \
	resource.hxx \
	session.hxx \
	atom-session.hxx

lib_LTLIBRARIES = libcmis-@LIBCMIS_API_VERSION@.la
libcmis_@LIBCMIS_API_VERSION@_la_SOURCES = \
	$(libcmis_headers_public) \
	folder.cxx \
	resource.cxx \
	atom-session.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -no-undefined

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(DEPS_LIBS)

libcmisdir = $(includedir)/libcmis-@LIBCMIS_API_VERSION@/libcmis
libcmis_HEADERS = $(libcmis_headers_public)

EXTRA_DIST = 
