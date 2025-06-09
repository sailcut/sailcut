/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
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

#ifndef FORMMAIN_H
#define FORMMAIN_H

#include <QMainWindow>
#include <QUrl>

#include "sailviewer-tabs.h"

// forward definitions
class QAction;
class QMenu;
class QMenuBar;
class QStatusBar;

class CSailApp;


/** The main dialog of the Sailcut application.
 */
class CFormMain : public QMainWindow
{
    Q_OBJECT

public:
    CFormMain(QWidget *parent = 0);
    bool load(const QString &filename);

protected:
    virtual QString getSaveFileName(const QString &filename) = 0;
    virtual bool read(const QString &filename) = 0;
    virtual bool write(const QString &filename) = 0;
    void addFileMenu(QMenu *menu);
    void addViewAction(QAction *action);
    void closeEvent(QCloseEvent *e);

private slots:
    void slotAbout();
    void slotAboutQt();

    void slotOpen();
    void slotOpenRecent();
    void slotSave();
    void slotSaveAs();

    void slotLanguage();

    void languageChange();
    void recentDocumentsChanged();

protected:
    CSailViewerTabs *tabs;

private:
    void setupMenuBar();

    /** The application */
    CSailApp *app;

    /** The current filename. */
    QString filename;

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
    /** The Language menu */
    QMenu *menuLanguage;

    /** The Most Recently Used files menu */
    QMenu *menuRecent;

    /** Display information about sailcut */
    QAction *actionAbout;
    /** Display information about Qt */
    QAction *actionAboutQt;

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
};

#endif
