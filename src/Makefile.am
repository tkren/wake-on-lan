DEFS = -DLOCALEDIR=\"$(datadir)/locale\" @DEFS@
INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/lib -I$(top_srcdir)/intl

bin_PROGRAMS = wol
bin_SCRIPTS = wol-dhcpdconf
wol_SOURCES = wol.c macfile.l net.c magic.c
wol_LDADD = @INTLLIBS@ @EXTRA_LIBS@ @LEXLIB@ $(top_builddir)/lib/libwrappers.a

MAINTAINERCLEANFILES = Makefile.in
