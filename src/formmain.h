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
#include <QUrl>

// forward definitions
class QMenuBar;
class QMenu;
class QStatusBar;
class QWorkspace;
class QSignalMapper;

class CPrefs;
class CSailApp;
class CFormDocument;

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
    void fileAccess(QString event, QString file);

private:
    void setupMenuBar();
    void setupMainWidget();
    CFormDocument *activeChild();

    // slots
protected slots:

    virtual void languageChange();

    virtual void slotAbout();
    virtual void slotAboutQt();

    virtual void slotNewRig();
    virtual void slotNewSail();
    virtual void slotOpen();
    virtual void slotOpenRecent();
    virtual void slotSave();
    virtual void slotSaveAs();

    virtual void slotHandbook();

    virtual void slotLanguage();
    virtual void slotUpdateMenus();

    // member variables
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

    // menus and action
    /** the File menu */
    QMenu *menuFile;
    /** the New submenu */
    QMenu *menuFileNew;
    /** the Help menu */
    QMenu *menuHelp;
    /** the Language menu */
    QMenu *menuLanguage;

    /** the Most Recently Used files menu */
    QMenu *menuRecent;

    /** display information about sailcut */
    QAction *actionAbout;
    /** display information about Qt */
    QAction *actionAboutQt;
    /** display the handbook */
    QAction *actionHandbook;
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
    /** close the active document */
    QAction *actionClose;
    /** view sail definition */
    QAction *actionViewDef;
    /** view sail mould */
    QAction *actionViewMould;
    /** view sail definition */
    QAction *actionViewPatch;
    /** view sail rig */
    QAction *actionViewRig;
    /** quit */
    QAction *actionQuit;
};

#endif
