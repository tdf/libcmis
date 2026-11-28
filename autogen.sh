#!/bin/sh

touch ChangeLog

LIBTOOLIZE=libtoolize
command -v glibtoolize >/dev/null 2>&1 && LIBTOOLIZE=glibtoolize

if [ ! -e ltmain.sh ]; then
    $LIBTOOLIZE
fi

aclocal -I m4
automake -a -c --foreign
autoconf
if test x$NOCONFIGURE = x; then ./configure "$@"; fi
