DEFS = -DLOCALEDIR=\"$(datadir)/locale\" @DEFS@
INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib -I$(top_srcdir)/intl

bin_PROGRAMS = wol
wol_SOURCES = wol.c net.c magic.c macfile.c
wol_LDADD = @INTLLIBS@ @EXTRA_LIBS@ $(top_builddir)/lib/libwrappers.a

MAINTAINERCLEANFILES = Makefile.in
