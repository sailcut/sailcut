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
#include <QMenuBar>
#include <QStatusBar>
#include <QSignalMapper>
#include <QDesktopServices>


/**
 * Constructs Sailcut CAD's main window.
 *
 * @param parent the parent widget
 */
CFormMain::CFormMain(QWidget *parent)
    : QMainWindow(parent)
{
    app = qobject_cast<CSailApp*>(qApp);
    setMinimumSize( QSize( 400, 400 ) );

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

    // load preferences
    makeMenuMru();

    // resize to preferred size
    resize(app->windowSize().expandedTo(minimumSizeHint()));
}


/**
 * This event is received when the user closes the dialog.
 */
void CFormMain::closeEvent(QCloseEvent *e)
{
    app->setWindowSize(size());
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

    for (unsigned int i = 0; i < app->prefs.mruDocuments.size(); i++)
    {
        menuRecent->addAction( app->prefs.mruDocuments[i], this, SLOT( slotOpenRecent() ) )->setData(i);
    }
}


/**
 * Creates the menu bar
 */
void CFormMain::setupMenuBar()
{
    // File menu
    menuFile = menuBar()->addMenu("");
    menuFileNew = menuFile->addMenu("");
    actionNewSail = menuFileNew->addAction("", app, SLOT(createSail()));
    actionNewHull = menuFileNew->addAction("", app, SLOT(createHull()));
    actionNewRig = menuFileNew->addAction("", app, SLOT(createRig()));
    actionNewBoat = menuFileNew->addAction("", app, SLOT(createBoat()));
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
    const QUrl handbook = app->findHandbook();
    if (!handbook.isEmpty())
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
        app->open(newfile);
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
    app->open(app->prefs.mruDocuments[index]);
}


/**
 * Saves the current sail definition to an XML file.
 */
void CFormMain::slotSave()
{
#if 0
    CFormDocument* child = activeChild();
    if (child->save())
    {
        QString filename = child->filename;
        statusbar->showMessage(tr("wrote '%1'").arg(filename));
        app->prefs.mruDocuments.touchEntry(filename);
        makeMenuMru();
    }
#endif
}


/**
 * Opens a dialog to select the XML to which the sail definition should be saved.
 */
void CFormMain::slotSaveAs()
{
#if 0
    CFormDocument* child = activeChild();
    if (child->saveAs())
    {
        QString filename = child->filename;
        statusbar->showMessage(tr("wrote '%1'").arg(filename));
        app->prefs.mruDocuments.touchEntry(filename);
        makeMenuMru();
    }
#endif
}


/**
 * Refresh the document-specific menus.
 */
void CFormMain::slotUpdateDocumentMenus()
{
#if 0
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
#endif
}
