#!/bin/sh
# do the funky auto* stuff

echo "Generating configure script using autoconf and automake"
aclocal
autoconf
autoheader
rm -rf intl
gettextize --force
automake --ignore-deps --add-missing --gnu
echo "Now you are ready to run ./configure with the desired options"
