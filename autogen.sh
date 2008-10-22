#! /bin/sh
if test -n "`which glibtoolize`"; then
  LIBTOOLIZE=glibtoolize
else
  LIBTOOLIZE=libtoolize
fi
if test -d /opt/local/share/aclocal; then
  ACLOCAL_FLAGS=-I/opt/local/share/aclocal
fi

cat admin/utils.m4.in admin/qt.m4.in admin/sailcut.m4.in > acinclude.m4

aclocal $ACLOCAL_FLAGS \
  && $LIBTOOLIZE --force --copy \
  && autoheader \
  && automake --add-missing --foreign --copy \
  && autoconf
