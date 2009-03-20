include_directories(${CMAKE_SOURCE_DIR}/src)

set(QT_USE_QTSVG 1)
set(QT_USE_QTXML 1)
include(${QT_USE_FILE})

#AM_CPPFLAGS = \
#	-I$(top_srcdir) -I$(srcdir)/sailcpp $(QT_CFLAGS) \
#	-DSAILCUT_DOCDIR=\"$(pkgdocdir)\" -DSAILCUT_DATADIR=\"$(pkgdatadir)\"

# the Qt UI files
set(sailcut_UIS formhulldefbase.ui formrigdefbase.ui formsaildefbase.ui)

#	ui_formhulldefbase.h
#	ui_formrigdefbase.h
#	ui_formsaildefbase.h
# generated source files
set(sailcut_MOC_HEADERS
	boatdef-panel.h
	formboat.h
	formdocument.h
	formhelp.h
	formhull.h
	formhulldef.h
	formmain.h
	formmould.h
	formpanelgroup.h
	formprint.h
	formrig.h
	formrigdef.h
	formsail.h
	formsaildef.h
	sailtreemodel.h
	sailviewer.h
	sailviewer-panel.h
	sailviewer-tabs.h
	widgetprofile.h
	widgetprofilevert.h)

# the application source, library search path, and link libraries
set(sailcut_SOURCES
	boatdef-panel.cpp
	formboat.cpp
	formhelp.cpp
	formhull.cpp
	formhulldef.cpp
	formmain.cpp
	formmould.cpp
	formpanelgroup.cpp
	formrig.cpp
	formprint.cpp
	formrigdef.cpp
	formsail.cpp
	formsaildef.cpp
	main.cpp
	prefs.cpp
	sailcutqt.cpp
	saildisp.cpp
	saildispgl.cpp
	saildisplabel.cpp
	saildoc.cpp
	sailpainter.cpp
	sailprinter.cpp
	sailtreeitem.cpp
	sailtreemodel.cpp
	sailviewer.cpp
	sailviewer-panel.cpp
	sailviewer-tabs.cpp
	sailwriter-carlson.cpp
	sailwriter-dxf.cpp
	sailwriter-hand.cpp
	sailwriter-svg.cpp
	sailwriter-txt.cpp
	widgetprofile.cpp
	widgetprofilevert.cpp)

qt4_wrap_cpp(sailcut_MOC_SOURCES ${sailcut_MOC_HEADERS})
add_executable(sailcut ${sailcut_SOURCES} ${sailcut_MOC_SOURCES})
target_link_libraries(sailcut sailcpp geocpp ${QT_LIBRARIES})
#sailcut_LDFLAGS = sailcpp/libsailcpp.a geocpp/libgeocpp.a $(QT_LIBS)

# rules for Qt metasources
#ui_%.h: %.ui
#	$(UIC) -o $@ $<

add_subdirectory(geocpp)
add_subdirectory(sailcpp)
