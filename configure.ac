dnl Process this file with autoconf to produce a configure script.

dnl we need autoconf 2.57 (this is tested)
AC_PREREQ(2.57)

AC_INIT(README)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST

PACKAGE=wol
MAJOR=0
MINOR=7
MICRO=0
VERSION=$MAJOR.$MINOR.$MICRO
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION")
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)

dnl ready for automake
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)


dnl check for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_AWK

do_pod2texi=""
AC_PATH_PROG(PL, perl, "")
if test x$PL = x ; then
  AC_MSG_WARN([*** Unable to find perl. Disabling manpage creation ***])
else
  AC_PATH_PROG(P2M, pod2man, "")
	if test x$P2M = x ; then
    AC_MSG_WARN([*** Unable to find pod2man. Disabling manpage creation ***])
	else
	  do_pod2texi="true"
	fi
fi
AM_CONDITIONAL([POD2TEXI], [test x$do_pod2texi = xtrue])



dnl check compiler characteristic
AC_C_CONST
AC_C_BIGENDIAN


dnl check data types
AC_CHECK_SIZEOF(unsigned char, 1)
AC_CHECK_TYPE([struct ether_addr], , , [#include <netinet/ether.h>])
if test "$ac_cv_type_struct_ether_addr" = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_STRUCT_ETHER_ADDR, 1)
fi



dnl check struct members
AC_CHECK_MEMBER([struct ether_addr.octet], , , [#include <netinet/ether.h>])
if test "$ac_cv_member_struct_ether_addr_octet" = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_STRUCT_ETHER_ADDR_OCTET, 1)
fi

AC_CHECK_MEMBER([struct ether_addr.ether_addr_octet], , ,
								[#include <netinet/ether.h>])
if test "$ac_cv_member_struct_ether_addr_ether_addr_octet" = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET, 1)
fi
								

dnl i18n
ALL_LINGUAS="de fr it es"
AM_GNU_GETTEXT


dnl check header files
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(unistd.h getopt.h sys/time.h locale.h libintl.h netinet/ether.h)


dnl check system functions
jm_PREREQ_ERROR dnl for GNU error
jm_FUNC_MALLOC	dnl for GNU xmalloc
jm_FUNC_REALLOC	dnl for GNU xmalloc
AM_FUNC_GETLINE dnl for GNU getline
AC_CHECK_FUNCS(usleep)
AC_CHECK_FUNCS(getopt_long) dnl for GNU getopt
AC_CHECK_FUNCS(getdelim)
AC_CHECK_FUNCS(ether_hostton)
AC_CHECK_FUNC(inet_aton, [], [
	dnl check libresolv for inet_aton() as seen on solaris
	AC_CHECK_LIB(resolv, inet_aton,
								EXTRA_LIBS="$EXTRA_LIBS -lresolv", EXTRA_LIBS="$EXTRA_LIBS")
])
AC_CHECK_FUNC(socket, [], [
	dnl check libsocket for socket() as seen on solaris
	AC_CHECK_LIB(socket, socket,
								EXTRA_LIBS="$EXTRA_LIBS -lsocket", EXTRA_LIBS="$EXTRA_LIBS")
])
AC_CHECK_FUNC(gethostbyname, [], [
  dnl check libnsl for gethostbyname() as seen on solaris
	AC_CHECK_LIB(nsl, gethostbyname,
	              EXTRA_LIBS="$EXTRA_LIBS -lnsl", EXTRA_LIBS="$EXTRA_LIBS")
])
AC_SUBST(EXTRA_LIBS)


dnl arguments
AC_MSG_CHECKING(wheter you want ANSI emulation)
AC_ARG_ENABLE(ansi,
[  --enable-ansi           enables strict ANSI checking [default=no]],
with_ansi=$enableval, with_ansi=no)
test "$with_ansi" = "yes" && CFLAGS="$CFLAGS -ansi -pedantic"
AC_MSG_RESULT($with_ansi)

dnl check for python
dnl AC_PATH_PROG(PY, python, "")
dnl if test x$PY = x ; then
dnl 	AC_MSG_WARN([
dnl *** Unable to find python.
dnl *** Because of that, you won't be able to run certain tests via 'make check'.])
dnl fi


dnl config.h.in defines
AC_DEFINE([HAVE_STRUCT_ETHER_ADDR], 0, [struct ether_addr])
AC_DEFINE([HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET], 0, [struct ether_addr.ether_addr_octet])
AC_DEFINE([HAVE_STRUCT_ETHER_ADDR_OCTET], 0, [struct ether_addr.octet])


dnl full featured warnings
CFLAGS="$CFLAGS -W -Wall -Wpointer-arith -Wimplicit-function-declaration"


dnl create the Makefiles
AC_OUTPUT([intl/Makefile po/Makefile.in
Makefile
demo/Makefile
doc/Makefile
include/Makefile
lib/Makefile
m4/Makefile
src/Makefile
src/wol-dhcpdconf
src/wol-bootptab], [chmod +x src/wol-dhcpdconf src/wol-bootptab])


dnl output configuration
echo "\

$PACKAGE configuration:

	prefix: $prefix
	exec_prefix: $exec_prefix

	bindir: $bindir
	libdir: $libdir
	datadir: $datadir
	mandir: $mandir
	infodir: $infodir

	EXTRA_LIBS: $EXTRA_LIBS
	ANSI emulation: $with_ansi
"
