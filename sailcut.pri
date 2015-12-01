SAILCUT_VERSION = 1.3.6

SAILCUT_SOURCE_TREE = $$PWD
isEmpty(SAILCUT_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    SAILCUT_BUILD_TREE = $$clean_path($$OUT_PWD)
    SAILCUT_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
BUILD_APP_PATH = $$SAILCUT_BUILD_TREE/bin
BUILD_DATA_PATH = $$SAILCUT_BUILD_TREE/share/sailcut

SAILCUT_DATA_PATH = ../share/sailcut
SAILCUT_DOC_PATH = ../share/doc/sailcut/html
