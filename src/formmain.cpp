/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
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

#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

#include "app.h"
#include "formmain.h"
#include "formsail.h"
#include "formhull.h"
#include "formrig.h"
#include "formpanelgroup.h"
#include "formboat.h"

#include "sailcut.xpm"


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
    setWindowIcon( QPixmap( (const char **)sailcut_xpm ) );

    // create status bar
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    // create menu bar
    setupMenuBar();

    // set language
    connect(qApp, SIGNAL(languageChanged()),
            this, SLOT(languageChange()));
    languageChange();

    // set recent documents
    connect(qApp, SIGNAL(recentDocumentsChanged()),
            this, SLOT(recentDocumentsChanged()));
    recentDocumentsChanged();

    // resize to preferred size
    resize(app->windowSize().expandedTo(minimumSizeHint()));
}


void CFormMain::addFileMenu(QMenu *menu)
{
    menuFile->insertMenu(actionFileSep, menu);
}


void CFormMain::addViewAction(QAction *action)
{
    menuView->insertAction(menuLanguage->menuAction(), action);
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


bool CFormMain::load(const QString &filename)
{
    if (read(filename)) {
        statusbar->showMessage(tr("loaded '%1'").arg(filename));
        app->addRecentDocument(filename);
        this->filename = filename;
        return true;
    } else {
        statusbar->showMessage( tr("error loading '%1'").arg(filename) );
        app->removeRecentDocument(filename);
        return false;
    }
}


/**
 * Creates the "Open Recent" menu from the Most Recently Used files list.
 */
void CFormMain::recentDocumentsChanged()
{
    menuRecent->clear();
    foreach (const QString &document, app->recentDocuments())
        menuRecent->addAction(document, this, SLOT(slotOpenRecent()))->setData(document);
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

    actionHandbook = menuHelp->addAction( "", app, SLOT( showHandbook() ) );
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
                        "Copyright (C) Robert Lainé & Jeremy Lainé"

                        "<p>For more information, visit the project's page at <a href=\"http://www.sailcut.com/\">http://www.sailcut.com/</a>.</p>"

                        "<p>Sailcut is a trademark registered by Robert Lainé</p>"

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
 * Switches the current language.
 */
void CFormMain::slotLanguage()
{
    QAction *a = qobject_cast<QAction *>(sender());
    if (a)
        app->setLanguage(a->data().toString());
}


/**
 * Displays a dialog box to open an XML sail definition.
 */
void CFormMain::slotOpen()
{
    QString filter = "Sailcut CAD files (";
    filter += QString("*") + CSailDefXmlWriter().fileExtension();
    filter += QString(" *") + CHullDefXmlWriter().fileExtension();
    filter += QString(" *") + CRigDefXmlWriter().fileExtension();
    filter += QString(" *") + CBoatDefXmlWriter().fileExtension();
    filter += QString(" *") + CPanelGroupXmlWriter().fileExtension();
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
    QAction *a = qobject_cast<QAction *>(sender());
    if (a)
        app->open(a->data().toString());
}


/**
 * Saves the current sail definition to an XML file.
 */
void CFormMain::slotSave()
{
    if (filename.isEmpty()) {
        slotSaveAs();
        return;
    }

    if (write(filename)) {
        statusbar->showMessage(tr("wrote '%1'").arg(filename));
        app->addRecentDocument(filename);
    } else {
        QMessageBox::information(0, tr("error"), tr("There was an error writing to the selected file."));
    }
}


/**
 * Opens a dialog to select the XML to which the sail definition should be saved.
 */
void CFormMain::slotSaveAs()
{
    const QString newfilename = getSaveFileName(filename);
    if (newfilename.isNull())
        return;

    if (write(newfilename)) {
        filename = newfilename;
        statusbar->showMessage(tr("wrote '%1'").arg(filename));
        app->addRecentDocument(filename);
    } else {
        QMessageBox::information(0, tr("error"), tr("There was an error writing to the selected file."));
    }
}
