AM_CPPFLAGS = -I$(top_srcdir)/src

noinst_HEADERS = geocpp.h geometry.h matrix.h subspace.h vector.h
noinst_LIBRARIES = libgeocpp.a
noinst_PROGRAMS = geotest

libgeocpp_a_SOURCES = geometry.cpp matrix.cpp subspace.cpp vector.cpp

geotest_SOURCES = geotest.cpp
geotest_LDADD = libgeocpp.a
