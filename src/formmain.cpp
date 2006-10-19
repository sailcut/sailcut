/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "formmain.h"
#include "formsail.h"
#include "formhelp.h"
#include "formrig.h"

#include "hullworker.h"
#include "sailcutqt.h"
#include "saildisp.h"
#include "saildoc.h"
#include "sailpainter.h"
#include "sailprinter.h"
#include "sailreader-xml.h"
#include "sailworker.h"
#include "sailwriter-carlson.h"
#include "sailwriter-dxf.h"
#include "sailwriter-hand.h"
#include "sailwriter-txt.h"
#include "sailwriter-xml.h"
#include "sailviewer-panel.h"

#include "../icons/sailcut.xpm"

#include <QDir>
#include <QLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QSignalMapper>
#include <QWorkspace>
#ifdef HAVE_QDESKTOPSERVICES
#include <QDesktopServices>
#endif


/**
 * Constructs Sailcut CAD's main window.
 *
 * @param myApp the Sailcut application
 * @param parent the parent widget
 */
CFormMain::CFormMain(CSailApp *myApp, QWidget *parent)
        : QMainWindow(parent), app(myApp), prefs(&myApp->prefs)
{
    setMinimumSize( QSize( 300, 220 ) );

    // locate Handbook
    app->loadTranslation(prefs->language);
    handbook = app->findHandbook(prefs->language);
    qDebug("handbook : %s", (const char*)handbook.toString().toLocal8Bit());

    // create main widget
    setupMainWidget();

    // create status bar
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    // create menu bar
    setupMenuBar();

    // set language
    languageChange();

    // set icon
    setWindowIcon( QPixmap( (const char **)sailcut_xpm ) );

    // update menus
    slotUpdateMenus();

    // resize to prefered size
    resize( QSize(prefs->mainWindowWidth,prefs->mainWindowHeight).expandedTo(minimumSizeHint()) );
}


/**
 * Returns the active document window.
 */
CFormDocument* CFormMain::activeChild()
{
    return qobject_cast<CFormDocument *>(workspace->activeWindow());
}


/**
 * This event is received when the user closes the dialog.
 */
void CFormMain::closeEvent(QCloseEvent *e)
{
    prefs->mainWindowHeight = height();
    prefs->mainWindowWidth = width();
    QMainWindow::closeEvent(e);
}


/**
 * This method is called whenever we read or write a sail definition.
 * It updates the Most Recently Used files list correspondingly.
 *
 * @param event The action performed on the file (read, write)
 * @param file The file that was accessed
 */
void CFormMain::fileAccess(QString event, QString file)
{
    statusbar->showMessage(event);
    prefs->mruSaildef.touchEntry(file);
    makeMenuMru();
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormMain::languageChange()
{
    setWindowTitle( "Sailcut CAD" );

    menuFile->setTitle( tr("&File") );

    menuFileNew->setTitle( tr("&New") );
    actionNewSail->setText( tr("Sail") );
    actionNewRig->setText( tr("Rig") );
    actionOpen->setText( tr("&Open") );
    menuRecent->setTitle( tr("Open &recent") );
    actionSave->setText( tr("&Save") );
    actionSaveAs->setText( tr("Save &As") );
    actionClose->setText( tr("&Close") );

    actionQuit->setText( tr("&Quit") );


    // Language menu
    menuLanguage->setTitle( tr("Language") );


    // Help menu
    menuHelp->setTitle( tr("&Help") );
    actionHandbook->setText( tr("Sailcut CAD &Handbook") );
    actionAboutQt->setText( tr("About &Qt") );
    actionAbout->setText( tr("About &Sailcut CAD") );

    // send changeLanguage to the tabs
    // TODO
}


/**
 * Creates the "Open Recent" menu from the Most Recently Used files list.
 */
void CFormMain::makeMenuMru()
{
    menuRecent->clear();

    for ( unsigned int i = 0; i < prefs->mruSaildef.size(); i++)
    {
        menuRecent->addAction( prefs->mruSaildef[i], this, SLOT( slotOpenRecent() ) )->setData(i);
    }
}


/**
 * Creates the main widget
 */
void CFormMain::setupMainWidget()
{
    workspace = new QWorkspace();
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget *)),
            this, SLOT(slotUpdateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
            workspace, SLOT(setActiveWindow(QWidget *)));    
}


/**
 * Creates the menu bar
 */
void CFormMain::setupMenuBar()
{
    // File menu
    menuFile = menuBar()->addMenu("");

    menuFileNew = menuFile->addMenu("");
    actionNewSail = menuFileNew->addAction("", this, SLOT( slotNewSail() ) );
    actionNewRig = menuFileNew->addAction("", this, SLOT( slotNewRig() ) );
    
    actionOpen = menuFile->addAction("", this, SLOT( slotOpen() ) );

    menuRecent = menuFile->addMenu("");

    menuFile->addSeparator();

    actionSave = menuFile->addAction("", this, SLOT( slotSave() ) );
    actionSaveAs = menuFile->addAction("", this, SLOT( slotSaveAs() ) );
    actionClose = menuFile->addAction( "", workspace, SLOT( closeActiveWindow() ) );

    menuFile->addSeparator();

    actionQuit = menuFile->addAction( "", this, SLOT( close() ) );

    // language submenu
    menuLanguage = menuBar()->addMenu("");

    // language text is not to be translated
    menuLanguage->addAction( "English", this, SLOT( slotLanguage() ) )->setData("en");
    menuLanguage->addAction( QString::fromUtf8("Français"), this, SLOT( slotLanguage() ) )->setData("fr");
    menuLanguage->addAction( "Nederlands", this, SLOT( slotLanguage() ) )->setData("nl");
    menuLanguage->addAction( "Deutsch", this, SLOT( slotLanguage() ) )->setData("de");
    menuLanguage->addAction( "Italiano", this, SLOT( slotLanguage() ) )->setData("it");
    menuLanguage->addAction( "Norsk", this, SLOT( slotLanguage() ) )->setData("no");
    menuLanguage->addAction( "Dansk", this, SLOT( slotLanguage() ) )->setData("dk");
    // menuLanguage->addAction( "Svenska", this, SLOT( slotLanguage() ) )->setData("sv");
    menuLanguage->addAction( QString::fromUtf8("Portugês"), this, SLOT( slotLanguage() ) )->setData("pt");
    // menuLanguage->addAction( "Español", this, SLOT( slotLanguage() ) )->setData("es");
    menuLanguage->addAction( "Russian", this, SLOT( slotLanguage() ) )->setData("ru");
    //menuLanguage->addAction( "Polish", this, SLOT( slotLanguage() ) )->setData("pl");

    // Help menu
    menuHelp = menuBar()->addMenu("");

    actionHandbook = menuHelp->addAction( "", this, SLOT( slotHandbook() ) );
    actionAboutQt = menuHelp->addAction( "", this, SLOT( slotAboutQt() ) );
    actionAbout = menuHelp->addAction( "", this, SLOT( slotAbout() ) );

}


/**
 * Opens a given sail file
 */
void CFormMain::show(const QString newname)
{
    // load preferences
    makeMenuMru();

    // load or create sail
/* 
    CSailDef newdef;
    filename = newname;
    if ( !filename.isNull() )
    {
        newdef = CSailDefXmlReader("saildef").read(filename);
        fileAccess(tr("loaded '%1'").arg(filename),filename);
    }
    else
    {
        statusbar->showMessage( tr("created new sail") );
    }
    setSailDef(newdef);
*/    
    QMainWindow::show();
}


/**
 * Displays the "About Sailcut CAD" dialog box
 */
void CFormMain::slotAbout()
{
    QMessageBox::about( this, tr("About Sailcut CAD"),

                        "<h2>Sailcut CAD"
#ifdef VERSION
                        " "VERSION
#endif
                        "</h2>"
                        "<p>Sailcut is a software for designing boat sails<br/>"
                        "(C) 1993-2006 Robert & Jeremy Laine"

                        "<p>For more information, visit the project's page at <a href=\"http://sailcut.sourceforge.net/\">http://sailcut.sourceforge.net/</a>.</p>"

                        "<p>Sailcut is a trademark registered by Robert Laine</p>"

                        "<table border=\"1\"><tr><td><small>"
                        "This program is free software; you can redistribute it and/or modify "
                        "it under the terms of the GNU General Public License as published by "
                        "the Free Software Foundation; either version 2 of the License, or "
                        "(at your option) any later version.<br/><br/>"

                        "This program is distributed in the hope that it will be useful, "
                        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                        "GNU General Public License for more details.<br/><br/>"

                        "You should have received a copy of the GNU General Public License "
                        "along with this program; if not, write to the Free Software "
                        "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA"
                        "</small></td></tr></table>"
                      );
}


/**
 * Displays the "About Qt" dialog box
 */
void CFormMain::slotAboutQt()
{
    QMessageBox::aboutQt( this );
}


/**
 * Display the Sailcut handbook.
 */
void CFormMain::slotHandbook()
{
    //qDebug("handbook : %s", (const char*)handbook.toLocal8Bit());
    if ( !handbook.isEmpty() )
    {
#ifdef HAVE_QDESKTOPSERVICES
        if (QDesktopServices::openUrl(handbook))
            return;
#endif
        CFormHelp(this , prefs , handbook).exec();
    }
}


/**
 * Switches the current language.
 */
void CFormMain::slotLanguage()
{
    QString locale;

    QAction *a = qobject_cast<QAction *>(sender());
    if ( !a )
        return;

    locale = a->data().toString();

    prefs->language = locale;
    app->loadTranslation(locale);

    // try to locate handbook
    handbook = app->findHandbook(locale);

    languageChange();
}


/**
 * Creates a new rig
 */
void CFormMain::slotNewRig()
{
    CFormRig *wnd = new CFormRig(prefs);
    workspace->addWindow(wnd);
    wnd->show();
}


/**
 * Creates a new sail
 */
void CFormMain::slotNewSail()
{
    CFormSail *wnd = new CFormSail(prefs);
    workspace->addWindow(wnd);
    wnd->show();
}


/**
 * Displays a dialog box to open an XML sail definition.
 */
void CFormMain::slotOpen()
{
/*    
    QString newname = CSailDefXmlReader("saildef").readDialog(newdef,filename);
    if ( !newname.isNull() )
    {
        filename = newname;
        fileAccess(tr("loaded '%1'").arg(filename), filename);
        setSailDef(newdef);
    }
*/    
}


/**
 * Opens a recently used sail definition.
 */
void CFormMain::slotOpenRecent()
{
    /*
    // retrieve the index of the MRU entry
    QAction *a = qobject_cast<QAction *>(sender());
    if ( !a )
        return;
    int index = a->data().toInt();

    filename = prefs->mruSaildef[index];
    try
    {
        setSailDef(CSailDefXmlReader("saildef").read(filename));
        fileAccess(tr("loaded '%1'").arg(filename), filename);
    }
    catch (CException e)
    {
        prefs->mruSaildef.removeEntry(filename);
        makeMenuMru();
        statusbar->showMessage( tr("error loading '%1'").arg(filename) );
    }*/
}


/**
 * Saves the current sail definition to an XML file.
 */
void CFormMain::slotSave()
{
    if (activeChild()->save())
    {
        statusBar()->showMessage("save");
    }
}


/**
 * Opens a dialog to select the XML to which the sail definition should be saved.
 */
void CFormMain::slotSaveAs()
{
    if (activeChild()->saveAs())
    {
        statusBar()->showMessage("save as");
    }
}


/**
 * Refresh the available menus.
 */
void CFormMain::slotUpdateMenus()
{
    bool hasChild = (activeChild() != 0);
    actionSave->setEnabled(hasChild);    
    actionSaveAs->setEnabled(hasChild);    
    actionClose->setEnabled(hasChild);    
}

