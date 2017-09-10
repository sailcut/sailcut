SAILCUT_VERSION = 1.4.1

SAILCUT_SOURCE_TREE = $$PWD
isEmpty(SAILCUT_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    SAILCUT_BUILD_TREE = $$clean_path($$OUT_PWD)
    SAILCUT_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
BUILD_APP_PATH = $$SAILCUT_BUILD_TREE/bin

mac {
    BUILD_DATA_PATH = "$$BUILD_APP_PATH/Sailcut CAD.app/Contents/Resources"
    SAILCUT_DATA_PATH = ../Resources
} else {
    BUILD_DATA_PATH = $$SAILCUT_BUILD_TREE/share/sailcut
    SAILCUT_DATA_PATH = ../share/sailcut
}

BUILD_DOC_PATH = $$SAILCUT_BUILD_TREE/share/doc/sailcut/html
SAILCUT_DOC_PATH = ../share/doc/sailcut/html
