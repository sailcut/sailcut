/*
 * Copyright (C) 1993-2008 Robert & Jeremy Laine
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
    void addChild(CFormDocument *child);
    void setupMenuBar();
    void setupMainWidget();
    CFormDocument *activeChild();

protected slots:

    virtual void languageChange();

    virtual void slotAbout();
    virtual void slotAboutQt();

    virtual void slotNew();
    virtual void slotOpen();
    virtual void slotOpenRecent();
    virtual void slotSave();
    virtual void slotSaveAs();

    virtual void slotHandbook();

    virtual void slotLanguage();
    virtual void slotUpdateDocumentMenus();
    virtual void slotUpdateWindowMenu();

protected:
    /** The workspace */
    QWorkspace *workspace;

    /** The signal mapper */
    QSignalMapper *windowMapper;

    /** The location of the Sailcut CAD Handbook */
    QUrl handbook;

    /** The application */
    CSailApp *app;

    /** The user preferences */
    CPrefs *prefs;

    /** The status bar */
    QStatusBar* statusbar;

    /** The File menu */
    QMenu *menuFile;
    /** The New submenu */
    QMenu *menuFileNew;
    /** The Help menu */
    QMenu *menuHelp;
    /** The View menu */
    QMenu *menuView;
    /** The Window menu */
    QMenu *menuWindow;
    /** The Language menu */
    QMenu *menuLanguage;

    /** The Most Recently Used files menu */
    QMenu *menuRecent;

    /** Display information about sailcut */
    QAction *actionAbout;
    /** Display information about Qt */
    QAction *actionAboutQt;
    /** Close the active document */
    QAction *actionClose;
    /** Close all documents */
    QAction *actionCloseAll;
    /** Tile document windows */
    QAction *actionTile;
    /** Cascade document windows */
    QAction *actionCascade;
    
    /** Display the handbook */
    QAction *actionHandbook;
    
    /** Create a new boat */
    QAction *actionNewBoat;
    /** Create a new hull */
    QAction *actionNewHull;
    /** Create a new rig */
    QAction *actionNewRig;
    /** Create a new sail */
    QAction *actionNewSail;
    /** Open an existing sail definition */
    QAction *actionOpen;
    
    /** Save the sail definition to a file */
    QAction *actionSave;
    /** Prompt for a filename then write the sail definition to a file */
    QAction *actionSaveAs;
    /** Final separator in the 'File' menu before 'Quit' */
    QAction *actionFileSep;
    
    /** View the sail definition */
    QAction *actionViewDef;
    /** View the sail mould */
    QAction *actionViewMould;
    /** View the sail patches definition */
    QAction *actionViewPatch;
    /** View the sail rig */
    QAction *actionViewRig;
    /** Separator in the 'Window' menu */
    QAction *actionWindowSep;
    /** Quit */
    QAction *actionQuit;

    /** Extra actions for the file menu */
    vector<QAction*> childFileActions;
    /** Extra actions for the view menu */
    vector<QAction*> childViewActions;
};

#endif
