dnl Process this file with autoconf to produce a configure script.

dnl we need autoconf 2.52 (this is tested)
AC_PREREQ(2.52)

AC_INIT(README)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST

PACKAGE=wol
MAJOR=0
MINOR=6
MICRO=0
VERSION=$MAJOR.$MINOR.$MICRO
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


dnl check compiler characteristic
AC_C_CONST
AC_C_BIGENDIAN


dnl check data types
AC_CHECK_SIZEOF(unsigned char, 1)


dnl i18n
dnl ALL_LINGUAS="de fr es it tr"
ALL_LINGUAS="de fr it"
AM_GNU_GETTEXT


dnl check header files
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(unistd.h getopt.h sys/time.h locale.h libintl.h)


dnl check system functions
jm_PREREQ_ERROR dnl for GNU error
jm_FUNC_MALLOC	dnl for GNU xmalloc
jm_FUNC_REALLOC	dnl for GNU xmalloc
AM_FUNC_GETLINE dnl for GNU getline
AC_CHECK_FUNCS(usleep)
AC_CHECK_FUNCS(getopt_long) dnl for GNU getopt
AC_CHECK_FUNCS(getdelim)
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
AC_SUBST(EXTRA_LIBS)


dnl arguments
AC_MSG_CHECKING(wheter you want ANSI emulation)
AC_ARG_ENABLE(ansi,
[  --enable-ansi           enables strict ANSI checking [default=no]],
with_ansi=$enableval, with_ansi=no)
test "$with_ansi" = "yes" && CFLAGS="$CFLAGS -ansi -pedantic"
AC_MSG_RESULT($with_ansi)

dnl checking for your awk
AC_MSG_CHECKING(path to your awk)
AC_ARG_WITH(myawk,
[  --with-awk=PATH_TO_AWK  path to your awk binary],
with_myawk=$withval, with_myawk="")
AC_MSG_RESULT($with_myawk)
if test x$with_myawk = x ; then
	AC_PATH_PROG(MY_AWK, awk, "")
else
	MY_AWK=$with_myawk
fi
if test x$MY_AWK = x ; then
	AC_MSG_ERROR([*** Unable to find your awk, specify it with --with-awk= ***])
fi
AC_SUBST(MY_AWK)


dnl full featured warnings
CFLAGS="$CFLAGS -W -Wall -Wpointer-arith -Wimplicit-function-declaration"


dnl create the Makefiles
AC_OUTPUT(
Makefile
demo/Makefile
doc/Makefile
include/Makefile
intl/Makefile
lib/Makefile
m4/Makefile
po/Makefile.in
src/Makefile
src/wol-dhcpdconf, [chmod +x src/wol-dhcpdconf])


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
	AWK binary: $MY_AWK
"
