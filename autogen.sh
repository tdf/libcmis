#!/bin/sh

touch ChangeLog

if [ ! -e ltmain.sh ]; then
    libtoolize
fi

aclocal -I m4
automake -a -c --foreign
autoconf
test x$NOCONFIGURE = x && ./configure $@
