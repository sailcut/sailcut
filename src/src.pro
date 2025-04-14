include(../sailcut.pri)

QT += openglwidgets printsupport svg xml

INCLUDEPATH += ../icons

TARGET = sailcut
DESTDIR = $$BUILD_APP_PATH
VERSION = $$SAILCUT_VERSION
DEFINES += SAILCUT_VERSION=\\\"$${SAILCUT_VERSION}\\\"
DEFINES += SAILCUT_DATA_PATH=\\\"$${SAILCUT_DATA_PATH}\\\"
DEFINES += SAILCUT_DOC_PATH=\\\"$${SAILCUT_DOC_PATH}\\\"
mac {
    TARGET = "Sailcut CAD"
}

FORMS += \
    formhulldefbase.ui \
    formrigdefbase.ui \
    formsaildefbase.ui

HEADERS += \
    geocpp/geocpp.h \
    geocpp/matrix.h \
    geocpp/matrix4x4.h \
    geocpp/rect.h \
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
    app.h \
    boatdef-panel.h \
    filewriter.h \
    formboat.h \
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
    geocpp/matrix.cpp \
    geocpp/matrix4x4.cpp \
    geocpp/rect.cpp \
    geocpp/subspace.cpp \
    geocpp/vector.cpp \
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
    app.cpp \
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

TRANSLATIONS = \
    ts/sailcut_de.ts \
    ts/sailcut_dk.ts \
    ts/sailcut_fr.ts \
    ts/sailcut_it.ts \
    ts/sailcut_nl.ts \
    ts/sailcut_no.ts \
    ts/sailcut_pt.ts \
    ts/sailcut_ru.ts

win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease

updateqm.input = TRANSLATIONS
updateqm.output = $$BUILD_DATA_PATH/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

# Installation
QMAKE_TARGET_BUNDLE_PREFIX="com.sailcut"
QMAKE_TARGET_COPYRIGHT="Copyright (c) Robert Lainé & Jeremy Lainé"
QMAKE_TARGET_PRODUCT="Sailcut CAD"
mac {
    ICON = ../icons/sailcut.icns
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/macdeployqt $$shell_quote($$DESTDIR/$${TARGET}.app)
} else:win32 {
    RC_ICONS = ../icons/sailcut.ico
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/windeployqt $$shell_quote($$DESTDIR/$${TARGET}.exe)
} else:unix {
    desktop.files = ../extras/sailcut.desktop
    desktop.path = $$PREFIX/share/applications

    icons.files = ../icons/sailcut.svg
    icons.path = $$PREFIX/share/icons/hicolor/scalable

    metainfo.files = ../extras/org.sailcut.cad.metainfo.xml
    metainfo.path = $$PREFIX/share/metainfo

    target.path = $$PREFIX/bin

    translations.files = $$BUILD_DATA_PATH
    translations.path = $$PREFIX/share

    INSTALLS += desktop icons metainfo target translations

    # Generating documentation requires some extra tools
    system(which fig2dev && which xsltproc) {
        docs.commands = $$SAILCUT_SOURCE_TREE/doc/makedocs $$BUILD_DOC_PATH
        docs.files = $$BUILD_DOC_PATH
        docs.path = $$PREFIX/share/doc/sailcut
        QMAKE_EXTRA_TARGETS += docs
        INSTALLS += docs
    }
}
