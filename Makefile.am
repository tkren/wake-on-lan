AUTOMAKE_OPTIONS = gnu no-dependencies 1.5
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath  autogen.sh debian

SUBDIRS = demo doc include lib src m4 po check

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
config.log config.status config.sub configure depcomp install-sh ltconfig \
ltmain.sh missing mkinstalldirs stamp-h.in ABOUT-NLS COPYING INSTALL \
po/Makefile.in.in po/Makefile.in po/ChangeLog po/wol.pot


dist-hook:
	rm -rf $(distdir)/debian/wol $(distdir)/debian/wol.substvars \
		$(distdir)/debian/CVS
