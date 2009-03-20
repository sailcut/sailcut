include_directories(${CMAKE_SOURCE_DIR}/icons ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(QT_USE_QTOPENGL 1)
set(QT_USE_QTSVG 1)
set(QT_USE_QTXML 1)
include(${QT_USE_FILE})

add_definitions(-DSAILCUT_VERSION="${SAILCUT_VERSION}")
add_definitions(-DSAILCUT_DATA_DIR="${CMAKE_INSTALL_PREFIX}/${SAILCUT_DATA_DIR}")
add_definitions(-DSAILCUT_DOC_DIR="${CMAKE_INSTALL_PREFIX}/${SAILCUT_DOC_DIR}")
if(QT_QTOPENGL_FOUND)
add_definitions(-DHAVE_QTOPENGL)
endif(QT_QTOPENGL_FOUND)

set(sailcut_UIS formhulldefbase.ui formrigdefbase.ui formsaildefbase.ui)

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
qt4_wrap_ui(sailcut_UI_SOURCES ${sailcut_UIS})
add_executable(sailcut ${sailcut_SOURCES} ${sailcut_MOC_SOURCES} ${sailcut_UI_SOURCES})
add_dependencies(sailcut translations)
target_link_libraries(sailcut sailcpp geocpp ${QT_LIBRARIES})

add_subdirectory(geocpp)
add_subdirectory(sailcpp)
add_subdirectory(ts)
