/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "formmain.h"
#include "formsail.h"
#include "formhull.h"
#include "formrig.h"
#include "formpanelgroup.h"
#include "formboat.h"

#include "sailcutqt.h"

#include "sailcut.xpm"

#include <QDebug>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QSignalMapper>
#include <QDesktopServices>


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

    // create main widget
    setupMainWidget();

    // create status bar
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    // create menu bar
    setupMenuBar();

    // set language
    connect(qApp, SIGNAL(languageChanged()),
            this, SLOT(languageChange()));
    languageChange();

    // set icon
    setWindowIcon( QPixmap( (const char **)sailcut_xpm ) );

    // update document-specific menus
    slotUpdateDocumentMenus();

    // resize to prefered size
    resize( QSize(prefs->mainWindowWidth,prefs->mainWindowHeight).expandedTo(minimumSizeHint()) );
}


/**
 * Add a document window and show it.
 */
void CFormMain::addChild(CFormDocument *child)
{
    bool max = ((activeChild() == NULL) || activeChild()->isMaximized());
    workspace->addSubWindow(child);
    if (max)
        child->showMaximized();
    else
        child->show();
}


/**
 * Returns the active document window.
 */
CFormDocument* CFormMain::activeChild()
{
    QMdiSubWindow *subWindow = workspace->activeSubWindow();
    return qobject_cast<CFormDocument *>(subWindow ? subWindow->widget() : 0);
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
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormMain::languageChange()
{
    setWindowTitle( "Sailcut CAD" );

    menuFile->setTitle( tr("&File") );

    menuFileNew->setTitle( tr("&New") );
    actionNewSail->setText( tr("sail") );
    actionNewHull->setText( tr("hull") );
    actionNewRig->setText( tr("rig") );
    actionNewBoat->setText( tr("boat") );
    actionOpen->setText( tr("&Open") );
    menuRecent->setTitle( tr("Open &recent") );
    actionSave->setText( tr("&Save") );
    actionSaveAs->setText( tr("Save &As") );

    actionQuit->setText( tr("&Quit") );

    // View menu
    menuView->setTitle( tr("&View") );
    menuLanguage->setTitle( tr("Language") );

    // Window menu
    menuWindow->setTitle( tr("&Window") );
    actionClose->setText( tr("&Close") );
    actionCloseAll->setText( tr("Close &All") );
    actionTile->setText( tr("&Tile") );
    actionCascade->setText( tr("&Cascade") );

    // Help menu
    menuHelp->setTitle( tr("&Help") );
    actionHandbook->setText( tr("Sailcut CAD &Handbook") );
    actionAboutQt->setText( tr("About &Qt") );
    actionAbout->setText( tr("About &Sailcut CAD") );

}


/**
 * Creates the "Open Recent" menu from the Most Recently Used files list.
 */
void CFormMain::makeMenuMru()
{
    menuRecent->clear();

    for (unsigned int i = 0; i < prefs->mruDocuments.size(); i++)
    {
        menuRecent->addAction( prefs->mruDocuments[i], this, SLOT( slotOpenRecent() ) )->setData(i);
    }
}


/**
 * Opens the specified document.
 *
 * @param filename
 */
void CFormMain::open(QString filename)
{
    CFormDocument *wnd;
    if (CSailDefXmlWriter().isDocument(filename))
    {
        wnd = new CFormSail(this);
    }
    else if (CHullDefXmlWriter().isDocument(filename))
    {
        wnd = new CFormHull(this);
    }
    else if (CBoatDefXmlWriter().isDocument(filename))
    {
        wnd = new CFormBoat(this);
    }
    else if (CRigDefXmlWriter().isDocument(filename))
    {
        wnd = new CFormRig(this);
    }
    else if (CPanelGroupXmlWriter().isDocument(filename))
    {
        wnd = new CFormPanelGroup(this);
    }
    else
    {
        statusbar->showMessage( tr("unknown document type '%1'").arg(filename) );
        return;
    }

    if (wnd->open(filename))
    {
        addChild(wnd);
        prefs->mruDocuments.touchEntry(filename);
        statusbar->showMessage(tr("loaded '%1'").arg(filename));
    }
    else
    {
        prefs->mruDocuments.removeEntry(filename);
        statusbar->showMessage( tr("error loading '%1'").arg(filename) );
        wnd->deleteLater();
    }
    makeMenuMru();
}


/**
 * Creates the main widget
 */
void CFormMain::setupMainWidget()
{
    workspace = new QMdiArea();
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(slotUpdateDocumentMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
            this, SLOT(slotSetActiveWindow(QWidget *)));
}


/**
 * Creates the menu bar
 */
void CFormMain::setupMenuBar()
{
    // File menu
    menuFile = menuBar()->addMenu("");
    menuFileNew = menuFile->addMenu("");
    actionNewSail = menuFileNew->addAction("", this, SLOT( slotNew() ) );
    actionNewHull = menuFileNew->addAction("", this, SLOT( slotNew() ) );
    actionNewRig = menuFileNew->addAction("", this, SLOT( slotNew() ) );
    actionNewBoat = menuFileNew->addAction("", this, SLOT( slotNew() ) );
    actionOpen = menuFile->addAction("", this, SLOT( slotOpen() ) );
    menuRecent = menuFile->addMenu("");
    menuFile->addSeparator();
    actionSave = menuFile->addAction("", this, SLOT( slotSave() ) );
    actionSaveAs = menuFile->addAction("", this, SLOT( slotSaveAs() ) );
    actionFileSep = menuFile->addSeparator();
    actionQuit = menuFile->addAction( "", this, SLOT( close() ) );

    // View menu
    menuView = menuBar()->addMenu("");
    menuLanguage = menuView->addMenu("");

    // Window menu
    menuWindow = menuBar()->addMenu("");
    actionClose = new QAction(this);
    connect(actionClose, SIGNAL( triggered() ), workspace, SLOT( closeActiveSubWindow() ) );
    actionCloseAll = new QAction(this);
    connect(actionCloseAll, SIGNAL( triggered() ), workspace, SLOT( closeAllSubWindows() ) );
    actionTile = new QAction(this);
    connect(actionTile, SIGNAL( triggered() ), workspace, SLOT( tileSubWindows() ) );
    actionCascade = new QAction(this);
    connect(actionCascade, SIGNAL( triggered() ), workspace, SLOT( cascadeSubWindows() ) );
    actionWindowSep = new QAction(this);
    actionWindowSep->setSeparator(true);
    connect(menuWindow, SIGNAL( aboutToShow() ), this, SLOT( slotUpdateWindowMenu() ) );


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
    // menuLanguage->addAction( QString::fromUtf8("Español"), this, SLOT( slotLanguage() ) )->setData("es");
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
void CFormMain::show(const QString filename)
{
    // load preferences
    makeMenuMru();

    // show main window
    QMainWindow::show();

    // load specified file or create empty sail
    if ( !filename.isNull() )
    {
        open(filename);
    }
    else
    {
        CFormSail *wnd = new CFormSail(this);
        addChild(wnd);
    }
}


/**
 * Displays the "About Sailcut CAD" dialog box
 */
void CFormMain::slotAbout()
{
    QMessageBox::about( this, tr("About Sailcut CAD"),

                        "<h2>Sailcut CAD " SAILCUT_VERSION "</h2>"
                        "<p>Sailcut is a software for designing boat sails<br/>"
                        "(C) 1993-2015 Robert & Jeremy Laine"

                        "<p>For more information, visit the project's page at <a href=\"http://www.sailcut.com/\">http://www.sailcut.com/</a>.</p>"

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
                        "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA"
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
    QUrl handbook = app->findHandbook(prefs->language);
    if ( !handbook.isEmpty() )
        QDesktopServices::openUrl(handbook);
}


/**
 * Switches the current language.
 */
void CFormMain::slotLanguage()
{
    QAction *a = qobject_cast<QAction *>(sender());
    if ( !a )
        return;

    app->setLanguage(a->data().toString());
}


/**
 * Creates a new document
 */
void CFormMain::slotNew()
{
    CFormDocument *wnd;

    QAction *a = qobject_cast<QAction *>(sender());
    if (a == actionNewSail)
    {
        wnd = new CFormSail(this);
    }
    else if (a == actionNewBoat)
    {
        wnd = new CFormBoat(this);
    }
    else if (a == actionNewRig)
    {
        wnd = new CFormRig(this);
    }
    else if (a == actionNewHull)
    {
        wnd = new CFormHull(this);
    }
    else
    {
        return;
    }

    addChild(wnd);
}


/**
 * Displays a dialog box to open an XML sail definition.
 */
void CFormMain::slotOpen()
{
    QString filter = "Sailcut CAD files (";
    filter += QString("*") + CSailDefXmlWriter().getExtension();
    filter += QString(" *") + CHullDefXmlWriter().getExtension();
    filter += QString(" *") + CRigDefXmlWriter().getExtension();
    filter += QString(" *") + CBoatDefXmlWriter().getExtension();
    filter += QString(" *") + CPanelGroupXmlWriter().getExtension();
    filter += ")";

    QString newfile = QFileDialog::getOpenFileName(0, tr("Open"), "", filter);
    if ( !newfile.isNull() )
        open(newfile);
}


/**
 * Opens a recently used sail definition.
 */
void CFormMain::slotOpenRecent()
{
    // retrieve the index of the MRU entry
    QAction *a = qobject_cast<QAction *>(sender());
    if ( !a )
        return;
    int index = a->data().toInt();
    open(prefs->mruDocuments[index]);
}


/**
 * Saves the current sail definition to an XML file.
 */
void CFormMain::slotSave()
{
    CFormDocument* child = activeChild();
    if (child->save())
    {
        QString filename = child->filename;
        statusbar->showMessage(tr("wrote '%1'").arg(filename));
        prefs->mruDocuments.touchEntry(filename);
        makeMenuMru();
    }
}


/**
 * Opens a dialog to select the XML to which the sail definition should be saved.
 */
void CFormMain::slotSaveAs()
{
    CFormDocument* child = activeChild();
    if (child->saveAs())
    {
        QString filename = child->filename;
        statusbar->showMessage(tr("wrote '%1'").arg(filename));
        prefs->mruDocuments.touchEntry(filename);
        makeMenuMru();
    }
}

void CFormMain::slotSetActiveWindow(QWidget *widget)
{
    workspace->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(widget));
}

/**
 * Refresh the document-specific menus.
 */
void CFormMain::slotUpdateDocumentMenus()
{
    bool hasChild = (activeChild() != 0);
    actionSave->setEnabled(hasChild);
    actionSaveAs->setEnabled(hasChild);

    // remove old extra menu entries
    unsigned int i;
    for (i = 0; i < childFileActions.size(); i++)
        menuFile->removeAction(childFileActions[i]);
    childFileActions.clear();
    for (i = 0; i < childViewActions.size(); i++)
        menuView->removeAction(childViewActions[i]);
    childViewActions.clear();

    // add new extra menu entries
    if (hasChild)
    {
        vector<QMenu*> menus = activeChild()->extraFileMenus;
        if (menus.size() > 0)
        {
            childFileActions.push_back(menuFile->insertSeparator(actionFileSep));
            for (i = 0; i < menus.size(); i++)
                childFileActions.push_back(menuFile->insertMenu(actionFileSep, menus[i]));
        }
        vector<QAction*> actions = activeChild()->extraViewActions;
        if (actions.size() > 0)
        {
            for (i = 0; i < actions.size(); i++)
            {
                childViewActions.push_back(actions[i]);
                menuView->insertAction(menuLanguage->menuAction(), actions[i]);
            }
            childViewActions.push_back(menuView->insertSeparator(menuLanguage->menuAction()));
        }

    }
}


/**
 * Refresh the "window" menu.
 */
void CFormMain::slotUpdateWindowMenu()
{
    menuWindow->clear();
    menuWindow->addAction(actionClose);
    menuWindow->addAction(actionCloseAll);
    menuWindow->addAction(actionTile);
    menuWindow->addAction(actionCascade);
    menuWindow->addAction(actionWindowSep);

    QList<QMdiSubWindow *> windows = workspace->subWindowList();
    actionWindowSep->setVisible(!windows.isEmpty());
    actionClose->setEnabled(!windows.isEmpty());
    actionCloseAll->setEnabled(!windows.isEmpty());
    actionTile->setEnabled(!windows.isEmpty());
    actionCascade->setEnabled(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i)
    {
        QMdiSubWindow *subWindow = windows.at(i);
        CFormDocument *child = qobject_cast<CFormDocument *>(subWindow->widget());

        QString text = QString("%1 %2").arg(i + 1).arg(child->windowTitle());
        if (!child->filename.isNull())
            text.append(QString(" - %1").arg(QFileInfo(child->filename).fileName()));

        QAction *action  = menuWindow->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == activeChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, subWindow);
    }
}


