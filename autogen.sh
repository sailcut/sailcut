#! /bin/sh
# $Id: autogen.sh,v 1.2 2005/09/03 14:28:58 jeremy_laine Exp $

cat admin/utils.m4.in admin/qt.m4.in admin/sailcut.m4.in > acinclude.m4

aclocal \
  && libtoolize --force --copy \
  && autoheader \
  && automake --add-missing --foreign --copy \
  && autoconf
