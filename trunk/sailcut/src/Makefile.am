SUBDIRS = geocpp ts

# set the include path for Qt
AM_CPPFLAGS = $(QT_INCLUDES) $(X_INCLUDES) $(GLINC) \
      -DSAILCUT_DOCDIR=\"$(pkgdocdir)\" -DSAILCUT_DATADIR=\"$(pkgdatadir)\"

bin_PROGRAMS = sailcut

# these are the headers
HEADER_FILES =  \
	disparea.h \
	fileio.h \
	filewriter.h \
	formdef.h \
	formhelp.h \
	formmain.h \
	formmould.h \
	formrig.h \
	panel.h \
	prefs.h \
	rigdef.h \
	rigdef-panel.h \
	sail.h \
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
	sailreader-xml.h \
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
	disparea.cpp \
	fileio.cpp \
	filewriter.cpp \
	formdef.cpp \
	formhelp.cpp \
	formmain.cpp \
	formmould.cpp \
	formrig.cpp \
	main.cpp \
	panel.cpp \
	prefs.cpp \
	rigdef.cpp \
	rigdef-panel.cpp \
	sail.cpp \
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
	sailviewer.cpp \
	sailviewer-panel.cpp \
	sailworker.cpp \
	sailwriter-carlson.cpp \
	sailwriter-dxf.cpp \
	sailwriter-hand.cpp \
	widgetprofile.cpp \
	widgetprofilevert.cpp

# the Qt UI files
UI_FILES = formdefbase.ui

# generated source files
BUILT_SOURCES = \
	formdefbase.h \
	formdefbase.cpp \
	formdef.moc.cpp \
	formhelp.moc.cpp \
	formmain.moc.cpp \
	formmould.moc.cpp \
	formrig.moc.cpp \
	rigdef-panel.moc.cpp \
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
