SUBDIRS = geocpp sailcpp ts

AM_CPPFLAGS = \
	-I$(top_srcdir)/icons $(QT_CFLAGS) -DHAVE_QTOPENGL \
	-DSAILCUT_DOC_DIR=\"$(pkgdocdir)\" -DSAILCUT_DATA_DIR=\"$(pkgdatadir)\" -DSAILCUT_VERSION=\"$(VERSION)\"

bin_PROGRAMS = sailcut

# these are the headers
noinst_HEADERS =  \
	boatdef-panel.h \
	filewriter.h \
	formboat.h \
	formdocument.h \
	formhull.h \
	formhulldef.h \
	formmain.h \
	formmould.h \
	formpanelgroup.h \
	formprint.h \
	formrig.h \
	formrigdef.h \
	formsail.h \
	formsaildef.h \
	prefs.h \
	sailcutqt.h \
	saildisp.h \
	saildispgl.h \
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

# the Qt UI files
UI_FILES = formhulldefbase.ui formrigdefbase.ui formsaildefbase.ui

# generated source files
BUILT_SOURCES = \
	ui_formhulldefbase.h \
	ui_formrigdefbase.h \
	ui_formsaildefbase.h \
	moc_boatdef-panel.cpp \
	moc_formboat.cpp \
	moc_formdocument.cpp \
	moc_formhull.cpp \
	moc_formhulldef.cpp \
	moc_formmain.cpp \
	moc_formmould.cpp \
	moc_formpanelgroup.cpp \
	moc_formprint.cpp \
	moc_formrig.cpp \
	moc_formrigdef.cpp \
	moc_formsail.cpp \
	moc_formsaildef.cpp \
	moc_sailtreemodel.cpp \
	moc_sailviewer.cpp \
	moc_sailviewer-panel.cpp \
	moc_sailviewer-tabs.cpp \
	moc_widgetprofile.cpp \
	moc_widgetprofilevert.cpp

# the application source, library search path, and link libraries
sailcut_SOURCES = \
	boatdef-panel.cpp \
	formboat.cpp \
	formhull.cpp \
	formhulldef.cpp \
	formmain.cpp \
	formmould.cpp \
	formpanelgroup.cpp \
	formrig.cpp \
	formprint.cpp \
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
nodist_sailcut_SOURCES = $(BUILT_SOURCES)

sailcut_LDFLAGS = sailcpp/libsailcpp.a geocpp/libgeocpp.a $(QT_LIBS)

# rules for Qt metasources
ui_%.h: %.ui
	$(UIC) -o $@ $<

moc_%.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = $(UI_FILES) 
