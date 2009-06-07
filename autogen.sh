#!/bin/sh
# First of all clean up the generated crud
rm -f configure config.log config.cache
rm -f config.status aclocal.m4
rm -rf libltdl
rm -f `find . -name 'Makefile.in'`
rm -f `find . -name 'Makefile'`

# Regenerate everything
libtoolize --force --copy 
gettextize --force --no-changelog 
aclocal -I m4 
autoheader
automake --add-missing --copy --foreign
autoconf 

echo
echo "Now type './configure' to prepare Adonthell for compilation."
echo "Afterwards, 'make' will build Adonthell."
echo
