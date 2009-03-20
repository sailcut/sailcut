# sailcut.m4 - Directory detection functions. -*-Autoconf-*-
dnl
dnl Copyright (C) 2005 Jeremy Lainé <jeremy.laine@m4x.org>
dnl See AUTHORS file for a full list of contributors.
dnl 
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
dnl

dnl ------------------------------------------------------------------------
dnl Determine the directory where the package's data is installed
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([JWB_CHECK_DATADIR],
[
    AC_ARG_WITH(datadir,
        [AC_HELP_STRING([--with-datadir=DIR],
	    [where package data is installed @<:@default=PREFIX/share/$1@:>@])],
        [ac_cv_with_datadir="$withval"])
	
    AC_MSG_CHECKING([for data directory])
    if test x"$ac_cv_with_datadir" = x; then
        pkgdatadir="${prefix}/share/$1"
    else
        pkgdatadir="$ac_cv_with_datadir"
    fi
    AC_MSG_RESULT($pkgdatadir)
    AC_SUBST(pkgdatadir)
])


dnl ------------------------------------------------------------------------
dnl Determine the directory where the package's documentation is installed
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([JWB_CHECK_DOCDIR],
[
    AC_ARG_WITH(docdir,
        [AC_HELP_STRING([--with-docdir=DIR],
	    [where package documentation is installed @<:@default=PREFIX/share/doc/$1@:>@])],
        [ac_cv_with_docdir="$withval"])
	
    AC_MSG_CHECKING([for documentation directory])
    if test x"$ac_cv_with_docdir" = x; then
        pkgdocdir="${prefix}/share/doc/$1"
    else
        pkgdocdir="$ac_cv_with_docdir"
    fi
    AC_MSG_RESULT($pkgdocdir)
    AC_SUBST(pkgdocdir)
])


dnl ------------------------------------------------------------------------
dnl Determine the directory where the package's icons are installed
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([JWB_CHECK_ICONDIR],
[
    AC_ARG_WITH(icondir,
        [AC_HELP_STRING([--with-icondir=DIR],
	    [where package icons are installed @<:@default=PREFIX/share/pixmaps@:>@])],
        [ac_cv_with_icondir="$withval"])
	
    AC_MSG_CHECKING([for icon directory])
    if test x"$ac_cv_with_icondir" = x; then
        pkgicondir="${prefix}/share/pixmaps"
    else
        pkgicondir="$ac_cv_with_icondir"
    fi
    AC_MSG_RESULT($pkgicondir)
    AC_SUBST(pkgicondir)
])

