SUBDIRS = libcmis libcmis-c 

if ENABLE_CLIENT
AM_CXXFLAGS = -I$(top_srcdir)/src/libcmis/ $(XML2_CFLAGS) $(BOOST_CPPFLAGS)

bin_PROGRAMS = cmis-client

cmis_client_SOURCES = cmis-client.cxx
cmis_client_LDADD = $(top_builddir)/src/libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(XML2_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIBS) \
	$(BOOST_DATE_TIME_LIBS) \
	$(JSON_LIBS)
endif
