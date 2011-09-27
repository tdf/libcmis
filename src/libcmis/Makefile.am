SUBDIRS = .

INCLUDES = -I$(top_srcdir)/include

libcmis_headers_public = \
	folder.hxx \
	resource.hxx \
	session.hxx \
	session-factory.hxx \
	atom-session.hxx \
	atom-folder.hxx \
	atom-resource.hxx

lib_LTLIBRARIES = libcmis-@LIBCMIS_API_VERSION@.la
libcmis_@LIBCMIS_API_VERSION@_la_SOURCES = \
	$(libcmis_headers_public) \
	session-factory.cxx \
	atom-folder.cxx \
	atom-resource.cxx \
	atom-session.cxx \
	atom-utils.hxx \
	atom-utils.cxx

libcmis_@LIBCMIS_API_VERSION@_la_LDFLAGS = -no-undefined

libcmis_@LIBCMIS_API_VERSION@_la_LIBADD = \
	$(DEPS_LIBS) \
	$(BOOST_SMART_PTR)

libcmisdir = $(includedir)/libcmis-@LIBCMIS_API_VERSION@/libcmis
libcmis_HEADERS = $(libcmis_headers_public)

EXTRA_DIST = 
