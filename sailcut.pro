include(sailcut.pri)

TEMPLATE = subdirs

SUBDIRS = src

# Package generation
mac {
    package.depends = first
    package.output = sailcut-$$SAILCUT_VERSION-mac.dmg
    package.commands =  \
        rm -f $$package.output; \
        ln -s /Applications $$BUILD_APP_PATH/Applications; \
        hdiutil create $$package.output -srcdir $$BUILD_APP_PATH -format UDBZ -volname \"Sailcut CAD $$SAILCUT_VERSION\"; \
        rm -f $$BUILD_APP_PATH/Applications
    QMAKE_EXTRA_TARGETS = package
} else:win32 {
    package.depends = first
    package.output = sailcut-$$SAILCUT_VERSION-win32.exe
    package.commands = makensis sailcut.nsi
    QMAKE_EXTRA_TARGETS = package

    NSI_HEADER = "!define PRODUCT_VERSION \"$$SAILCUT_VERSION\""
    NSI_HEADER += "!define PRODUCT_LICENSE \"$$system_path($$SAILCUT_SOURCE_TREE/COPYING)\""
    NSI_HEADER += "!define PRODUCT_OUTPUT \"$$system_path($$package.output)\""
    NSI_BODY = $$cat($$SAILCUT_SOURCE_TREE/sailcut.nsi.in, blob)
    write_file($$SAILCUT_BUILD_TREE/sailcut.nsi, NSI_HEADER)
    write_file($$SAILCUT_BUILD_TREE/sailcut.nsi, NSI_BODY, append)
    QMAKE_CLEAN += sailcut.nsi
} else:unix {
    system(which fig2dev && which xsltproc) {
        docs.commands = $$PWD/doc/makedocs $$BUILD_DOC_PATH
        docs.files = $$BUILD_DOC_PATH
        docs.path = $$PREFIX/share/doc/sailcut
        QMAKE_EXTRA_TARGETS += docs
        INSTALLS += docs
    }
}
