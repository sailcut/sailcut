#! /bin/sh
if test -n "`which glibtoolize`"; then
  LIBTOOLIZE=glibtoolize
else
  LIBTOOLIZE=libtoolize
fi
if test -d /opt/local/share/aclocal; then
  ACLOCAL_FLAGS=-I/opt/local/share/aclocal
fi

aclocal $ACLOCAL_FLAGS \
  && $LIBTOOLIZE --force --copy \
  && autoheader \
  && automake --add-missing --foreign --copy \
  && autoconf
