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

#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QUrl>
#include "formdocument.h"

// forward definitions
class QAction;
class QMenu;
class QMenuBar;
class QStatusBar;
class QWorkspace;
class QSignalMapper;

class CPrefs;
class CSailApp;


/** The main dialog of the Sailcut application.
 */
class CFormMain : public QMainWindow
{
    Q_OBJECT

public:
    // construction, destruction
    CFormMain(CSailApp *myApp, QWidget *parent = 0);

    void show(const QString newname = QString::null);

protected:
    void closeEvent( QCloseEvent * e);
    void makeMenuMru();
    void open(QString filename);

private:
    void setupMenuBar();
    void setupMainWidget();
    CFormDocument *activeChild();

protected slots:

    virtual void languageChange();

    virtual void slotAbout();
    virtual void slotAboutQt();

    virtual void slotNewHull();
    virtual void slotNewRig();
    virtual void slotNewSail();
    virtual void slotOpen();
    virtual void slotOpenRecent();
    virtual void slotSave();
    virtual void slotSaveAs();

    virtual void slotHandbook();

    virtual void slotLanguage();
    virtual void slotUpdateDocumentMenus();
    virtual void slotUpdateWindowMenu();

protected:
    /** the workspace */
    QWorkspace *workspace;

    /** signal mapper */
    QSignalMapper *windowMapper;

    /** the location of the Sailcut CAD Handbook */
    QUrl handbook;

    /** the application */
    CSailApp *app;

    /** the user preferences */
    CPrefs *prefs;

    /** the status bar */
    QStatusBar* statusbar;

    /** the File menu */
    QMenu *menuFile;
    /** the New submenu */
    QMenu *menuFileNew;
    /** the Help menu */
    QMenu *menuHelp;
    /** the View menu */
    QMenu *menuView;
    /** the Window menu */
    QMenu *menuWindow;
    /** the Language menu */
    QMenu *menuLanguage;

    /** the Most Recently Used files menu */
    QMenu *menuRecent;

    /** display information about sailcut */
    QAction *actionAbout;
    /** display information about Qt */
    QAction *actionAboutQt;
    /** close the active document */
    QAction *actionClose;
    /** close all documents */
    QAction *actionCloseAll;
    /** tile document windows */
    QAction *actionTile;
    /** cascade document windows */
    QAction *actionCascade;
    /** display the handbook */
    QAction *actionHandbook;
    /** create a new hull */
    QAction *actionNewHull;
    /** create a new rig */
    QAction *actionNewRig;
    /** create a new sail */
    QAction *actionNewSail;
    /** open an existing sail definition */
    QAction *actionOpen;
    /** sail the sail definition to a file */
    QAction *actionSave;
    /** prompt for a filename then write the sail definition to a file */
    QAction *actionSaveAs;
    /** final separator in the 'File' menu before 'Quit' */
    QAction *actionFileSep;
    /** view sail definition */
    QAction *actionViewDef;
    /** view sail mould */
    QAction *actionViewMould;
    /** view sail definition */
    QAction *actionViewPatch;
    /** view sail rig */
    QAction *actionViewRig;
    /** separator in the 'Window' menu */
    QAction *actionWindowSep;
    /** quit */
    QAction *actionQuit;

    /** extra actions for the file menu */
    vector<QAction*> childFileActions;
    /** extra actions for the view menu */
    vector<QAction*> childViewActions;
};

#endif
