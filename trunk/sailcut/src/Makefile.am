SUBDIRS = geocpp ts

AM_CPPFLAGS = \
	-I$(top_srcdir) $(QT_INCLUDES) $(X_INCLUDES) $(GLINC) \
	-DSAILCUT_DOCDIR=\"$(pkgdocdir)\" -DSAILCUT_DATADIR=\"$(pkgdatadir)\"

bin_PROGRAMS = sailcut

# these are the headers
HEADER_FILES =  \
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
	formrigdef.h \
	formsail.h \
	formsaildef.h \
	hulldef.h \
	hullworker.h \
	panel.h \
	panelgroup.h \
	prefs.h \
	rigdef.h \
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
	sailworker.h \
	sailwriter-carlson.h \
	sailwriter-dxf.h \
	sailwriter-hand.h \
	sailwriter-txt.h \
	sailwriter-xml.h \
	widgetprofile.h \
	widgetprofilevert.h

# the C++ source files
CPP_FILES = \
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
	sailworker.cpp \
	sailwriter-carlson.cpp \
	sailwriter-dxf.cpp \
	sailwriter-hand.cpp \
	sailwriter-txt.cpp \
	widgetprofile.cpp \
	widgetprofilevert.cpp

# the Qt UI files
UI_FILES = formhulldefbase.ui formrigdefbase.ui formsaildefbase.ui

# generated source files
BUILT_SOURCES = \
	boatdef-panel.moc.cpp \
	formboat.moc.cpp \
	formhelp.moc.cpp \
	formhull.moc.cpp \
	formhulldefbase.h \
	formhulldefbase.cpp \
	formhulldef.moc.cpp \
	formmain.moc.cpp \
	formmould.moc.cpp \
	formpanelgroup.moc.cpp \
	formrigdefbase.h \
	formrigdefbase.cpp \
	formrigdef.moc.cpp \
	formsail.moc.cpp \
	formsaildefbase.h \
	formsaildefbase.cpp \
	formsaildef.moc.cpp \
	sailtreemodel.moc.cpp \
	sailviewer.moc.cpp \
	sailviewer-panel.moc.cpp \
	widgetprofile.moc.cpp \
	widgetprofilevert.moc.cpp

# the application source, library search path, and link libraries
sailcut_SOURCES = $(HEADER_FILES) $(CPP_FILES)
nodist_sailcut_SOURCES = $(BUILT_SOURCES)

sailcut_LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS) $(GL_LDFLAGS)
sailcut_LDADD   = geocpp/libgeocpp.a $(LIB_QT) $(LIB_QTXML) $(LIB_QTOPENGL) $(GLLIB)

# rules for Qt metasources
%.cpp: %.ui %.h
	$(UIC) $< > $@; ret=$$?; \
	if test "$$ret" != 0; then rm -f $@ ; exit $$ret ; fi

%.h: %.ui
	$(UIC) -o $@ $<

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = $(UI_FILES) 
