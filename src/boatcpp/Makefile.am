AM_CPPFLAGS = -I$(top_srcdir)/src $(QT_INCLUDES)

noinst_HEADERS = panel.h panelgroup.h saildef.h sailcalc.h sailmould.h sailworker.h
noinst_LIBRARIES = libsailcpp.a
noinst_PROGRAMS = sailtest

libsailcpp_a_SOURCES = panel.cpp panelgroup.cpp saildef.cpp sailcalc.cpp sailmould.cpp sailworker.cpp

sailtest_SOURCES = sailtest.cpp
sailtest_LDFLAGS = $(QT_LDFLAGS)
sailtest_LDADD   = ../geocpp/libgeocpp.a libsailcpp.a $(LIB_QT)

