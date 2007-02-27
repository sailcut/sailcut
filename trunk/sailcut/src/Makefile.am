SUBDIRS = geocpp ts

AM_CPPFLAGS = \
	-I$(top_srcdir) $(QT_INCLUDES) $(X_INCLUDES) $(GLINC) \
	-DSAILCUT_DOCDIR=\"$(pkgdocdir)\" -DSAILCUT_DATADIR=\"$(pkgdatadir)\"

bin_PROGRAMS = sailcut

# these are the headers
noinst_HEADERS =  \
	boatdef.h \
	boatdef-panel.h \
	disparea.h \
	filewriter.h \
	formboat.h \
	formdocument.h \
	formhelp.h \
	formhull.h \
	formhulldef.h \
	formmain.h \
	formmould.h \
	formpanelgroup.h \
	formrig.h \
	formrigdef.h \
	formsail.h \
	formsaildef.h \
	hulldef.h \
	hullworker.h \
	panel.h \
	panelgroup.h \
	prefs.h \
	rigdef.h \
	rigworker.h \
	sailcalc.h \
	sailcutqt.h \
	saildef.h \
	saildisp.h \
	saildispgl.h \
	saildisplabel.h \
	saildoc.h \
	sailmould.h \
	sailpainter.h \
	sailprinter.h \
	sailtreeitem.h \
	sailtreemodel.h \
	sailviewer.h \
	sailviewer-panel.h \
	sailviewer-tabs.h \
	sailworker.h \
	sailwriter-carlson.h \
	sailwriter-dxf.h \
	sailwriter-hand.h \
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
	moc_formhelp.cpp \
	moc_formhull.cpp \
	moc_formhulldef.cpp \
	moc_formmain.cpp \
	moc_formmould.cpp \
	moc_formpanelgroup.cpp \
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
	boatdef.cpp \
	boatdef-panel.cpp \
	disparea.cpp \
	formboat.cpp \
	formhelp.cpp \
	formhull.cpp \
	formhulldef.cpp \
	formmain.cpp \
	formmould.cpp \
	formpanelgroup.cpp \
	formrig.cpp \
	formrigdef.cpp \
	formsail.cpp \
	formsaildef.cpp \
	hulldef.cpp \
	hullworker.cpp \
	main.cpp \
	panel.cpp \
	panelgroup.cpp \
	prefs.cpp \
	rigdef.cpp \
	rigworker.cpp \
	sailcalc.cpp \
	sailcutqt.cpp \
	saildef.cpp \
	saildisp.cpp \
	saildispgl.cpp \
	saildisplabel.cpp \
	saildoc.cpp \
	sailmould.cpp \
	sailpainter.cpp \
	sailprinter.cpp \
	sailtreeitem.cpp \
	sailtreemodel.cpp \
	sailviewer.cpp \
	sailviewer-panel.cpp \
	sailviewer-tabs.cpp \
	sailworker.cpp \
	sailwriter-carlson.cpp \
	sailwriter-dxf.cpp \
	sailwriter-hand.cpp \
	sailwriter-txt.cpp \
	widgetprofile.cpp \
	widgetprofilevert.cpp
nodist_sailcut_SOURCES = $(BUILT_SOURCES)

sailcut_LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS) $(GL_LDFLAGS)
sailcut_LDADD   = geocpp/libgeocpp.a $(LIB_QT) $(LIB_QTXML) $(LIB_QTOPENGL) $(GLLIB)

# rules for Qt metasources
ui_%.h: %.ui
	$(UIC) -o $@ $<

moc_%.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = $(UI_FILES) 
