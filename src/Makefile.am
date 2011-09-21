SUBDIRS = libcmis .

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = test-atom

test_atom_SOURCES = test-atom.cxx
test_atom_LDADD = libcmis/libcmis-@LIBCMIS_API_VERSION@.la \
	$(DEPS_LIBS)
test_atom_CPPFLAGS = -I$(top_builddir)/lib/libcmis/libcmis.la
