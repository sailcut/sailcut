include_directories(${CMAKE_SOURCE_DIR}/icons)

relative_path(SAILCUT_DATA_PATH ${SAILCUT_RUNTIME_DIR} ${SAILCUT_DATA_DIR})
relative_path(SAILCUT_DOC_PATH ${SAILCUT_RUNTIME_DIR} ${SAILCUT_DOC_DIR})

add_definitions(-DSAILCUT_VERSION="${SAILCUT_VERSION}")
add_definitions(-DSAILCUT_DATA_PATH="${SAILCUT_DATA_PATH}")
add_definitions(-DSAILCUT_DOC_PATH="${SAILCUT_DOC_PATH}")

# OpenGL dependencies
if(APPLE)
    set(OPENGL_LIBRARIES "-framework OpenGL -framework AGL")
elseif(WIN32)
    set(OPENGL_LIBRARIES -lopengl32 -lglu32)
else()
    set(OPENGL_LIBRARIES -lGL)
endif()

set(sailcut_UIS formhulldefbase.ui formrigdefbase.ui formsaildefbase.ui)

set(sailcut_SOURCES
    boatdef-panel.cpp
    formboat.cpp
    formdocument.h
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

if(WIN32)
    enable_language(RC)
    set(SAILCUT_VERSION_WIN32 "${SAILCUT_VERSION_MAJOR},${SAILCUT_VERSION_MINOR},${SAILCUT_VERSION_PATCH},0")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/sailcut.rc.in ${CMAKE_CURRENT_BINARY_DIR}/sailcut.rc)
    set(sailcut_SOURCES ${sailcut_SOURCES} sailcut.rc)
endif(WIN32)

if(USE_QT5)
    qt5_wrap_ui(sailcut_UI_SOURCES ${sailcut_UIS})
else()
    qt4_wrap_ui(sailcut_UI_SOURCES ${sailcut_UIS})
endif()
add_executable(sailcut ${sailcut_SOURCES} ${sailcut_UI_SOURCES})
add_dependencies(sailcut documentation)
add_dependencies(sailcut translations)
if (WIN32)
    set_target_properties(sailcut PROPERTIES LINK_FLAGS "-mwindows")
endif(WIN32)
if(USE_QT5)
    target_link_libraries(sailcut sailcpp geocpp Qt5::OpenGL Qt5::PrintSupport Qt5::Svg Qt5::Widgets Qt5::Xml ${OPENGL_LIBRARIES})
else()
    target_link_libraries(sailcut sailcpp geocpp Qt4::QtGui Qt4::QtOpenGL Qt4::QtSvg Qt4::QtXml ${OPENGL_LIBRARIES})
endif()

ship_depends(sailcut)
install(TARGETS sailcut DESTINATION ${SAILCUT_RUNTIME_DIR})

add_subdirectory(geocpp)
add_subdirectory(sailcpp)
add_subdirectory(ts)
