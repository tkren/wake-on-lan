#!/bin/sh
# do the funky auto* stuff

echo "Generating configure script using autoconf and automake"
set +x
aclocal -I m4
autoconf
autoheader
rm -rf intl
rm -f po/ChangeLog
gettextize --force
automake --ignore-deps --add-missing --gnu
set -x
echo "Now you are ready to run ./configure with the desired options"
