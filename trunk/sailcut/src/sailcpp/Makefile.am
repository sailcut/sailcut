AM_CPPFLAGS = -I$(top_srcdir)/src

noinst_LIBRARIES = libsailcpp.a
noinst_PROGRAMS = sailtest

noinst_HEADERS = \
	boatdef.h \
	hulldef.h \
	hullworker.h \
	panelgroup.h \
	panel.h \
	rigdef.h \
	rigworker.h \
	sailcalc.h \
	saildef.h \
	sailmould.h \
	sailworker.h

libsailcpp_a_SOURCES = \
	boatdef.cpp \
	hulldef.cpp \
	hullworker.cpp \
	panel.cpp \
	panelgroup.cpp \
	rigdef.cpp \
	rigworker.cpp \
	sailcalc.cpp \
	saildef.cpp \
	sailmould.cpp \
	sailworker.cpp

sailtest_SOURCES = sailtest.cpp
sailtest_LDADD   = libsailcpp.a ../geocpp/libgeocpp.a

