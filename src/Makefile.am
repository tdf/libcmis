SUBDIRS = libcmis .

INCLUDES = -I$(top_srcdir)/include

if ENABLE_CLIENT
bin_PROGRAMS = cmis-client

cmis_client_SOURCES = cmis-client.cxx
cmis_client_LDADD = libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(DEPS_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIBS)
cmis_client_CPPFLAGS = -I$(top_builddir)/lib/libcmis/libcmis.la
endif
