include(../sailcut.pri)

QT += opengl printsupport svg xml

INCLUDEPATH += ../icons

TARGET = sailcut
DESTDIR = $$SAILCUT_APP_PATH
VERSION = $$SAILCUT_VERSION
DEFINES += SAILCUT_VERSION=\\\"$${SAILCUT_VERSION}\\\"
DEFINES += SAILCUT_DATA_PATH=\\\"$${SAILCUT_DATA_PATH}\\\"
DEFINES += SAILCUT_DOC_PATH=\\\"$${SAILCUT_DOC_PATH}\\\"

FORMS += \
    formhulldefbase.ui \
    formrigdefbase.ui \
    formsaildefbase.ui

HEADERS += \
    geocpp/geocpp.h \
    geocpp/geometry.h \
    geocpp/matrix.h \
    geocpp/subspace.h \
    geocpp/vector.h \
    sailcpp/boatdef.h \
    sailcpp/hulldef.h \
    sailcpp/hullworker.h \
    sailcpp/panelgroup.h \
    sailcpp/panel.h \
    sailcpp/rigdef.h \
    sailcpp/rigworker.h \
    sailcpp/sailcalc.h \
    sailcpp/saildef.h \
    sailcpp/sailmould.h \
    sailcpp/sailworker.h \
    boatdef-panel.h \
    filewriter.h \
    formboat.h \
    formdocument.h \
    formhulldef.h \
    formhull.h \
    formmain.h \
    formmould.h \
    formpanelgroup.h \
    formprint.h \
    formrigdef.h \
    formrig.h \
    formsaildef.h \
    formsail.h \
    prefs.h \
    sailcutqt.h \
    saildispgl.h \
    saildisp.h \
    saildisplabel.h \
    saildoc.h \
    sailpainter.h \
    sailprinter.h \
    sailtreeitem.h \
    sailtreemodel.h \
    sailviewer.h \
    sailviewer-panel.h \
    sailviewer-tabs.h \
    sailwriter-carlson.h \
    sailwriter-dxf.h \
    sailwriter-hand.h \
    sailwriter-svg.h \
    sailwriter-txt.h \
    sailwriter-xml.h \
    widgetprofile.h \
    widgetprofilevert.h

SOURCES += \
    geocpp/geometry.cpp \
    geocpp/matrix.cpp \
    geocpp/subspace.cpp \
    sailcpp/boatdef.cpp \
    sailcpp/hulldef.cpp \
    sailcpp/hullworker.cpp \
    sailcpp/panel.cpp \
    sailcpp/panelgroup.cpp \
    sailcpp/rigdef.cpp \
    sailcpp/rigworker.cpp \
    sailcpp/sailcalc.cpp \
    sailcpp/saildef.cpp \
    sailcpp/sailmould.cpp \
    sailcpp/sailworker.cpp \
    boatdef-panel.cpp \
    formboat.cpp \
    formhull.cpp \
    formhulldef.cpp \
    formmain.cpp \
    formmould.cpp \
    formpanelgroup.cpp \
    formprint.cpp \
    formrig.cpp \
    formrigdef.cpp \
    formsail.cpp \
    formsaildef.cpp \
    main.cpp \
    prefs.cpp \
    sailcutqt.cpp \
    saildisp.cpp \
    saildispgl.cpp \
    saildisplabel.cpp \
    saildoc.cpp \
    sailpainter.cpp \
    sailprinter.cpp \
    sailtreeitem.cpp \
    sailtreemodel.cpp \
    sailviewer.cpp \
    sailviewer-panel.cpp \
    sailviewer-tabs.cpp \
    sailwriter-carlson.cpp \
    sailwriter-dxf.cpp \
    sailwriter-hand.cpp \
    sailwriter-svg.cpp \
    sailwriter-txt.cpp \
    widgetprofile.cpp \
    widgetprofilevert.cpp

# Installation
QMAKE_TARGET_BUNDLE_PREFIX="com.sailcut"
QMAKE_TARGET_COPYRIGHT="Copyright (c) 1993-2015 Robert & Jeremy Laine"
QMAKE_TARGET_PRODUCT="Sailcut CAD"
mac {
    ICON = ../icons/sailcut.icns
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/macdeployqt $$DESTDIR/sailcut.app
} else:win32 {
    RC_ICONS = ../icons/sailcut.ico
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/windeployqt $$DESTDIR/sailcut.exe
}
