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

#ifndef FORMSAIL_H
#define FORMSAIL_H

#include <QMainWindow>
#include <QMessageBox>
#include <QUrl>

#include "sailcalc.h"

#include "panelgroup.h"
#include "hulldef.h"
#include "saildef.h"

// forward definitions
class QApplication;
class QMenuBar;
class QGridLayout;
class QMenu;
class QStatusBar;
class QTabWidget;
class QTranslator;
class CSailViewerPanel;
class CPrefs;
class CSailApp;


/** The main dialog of the Sailcut application.
 */
class CFormSail : public QMainWindow
{
    Q_OBJECT

public:
    // construction, destruction
    CFormSail(CSailApp *myApp, QWidget *parent = 0);

    void show(const QString newname = QString::null);
    void setSailDef(const CSailDef newdef);

protected:
    void closeEvent( QCloseEvent * e);
    void keyPressEvent ( QKeyEvent * e );
    void makeMenuMru();
    void fileAccess(QString event, QString file);

    void setupMenuBar();
    void setupMainWidget();

    // slots
protected slots:

    virtual void languageChange();

    virtual void slotDef();
    virtual void slotExportDXF();
    virtual void slotExportTXT();
    virtual void slotExportXML();
    virtual void slotExportFlatCarlson();
    virtual void slotExportFlatDXF();
    virtual void slotExportFlatDXFBlocks();
    virtual void slotExportFlatHand();
    virtual void slotExportFlatTXT();
    virtual void slotExportFlatXML();
    virtual void slotMould();
    virtual void slotOpen();
    virtual void slotOpenRecent();
    virtual void slotSave();
    virtual void slotSaveAs();

    virtual void slotPrintData();
    virtual void slotPrintDwg();
    virtual void slotPrintDev();

    // member variables
protected:
    /** the application */
    CSailApp *app;
    /** the user preferences */
    CPrefs *prefs;

    /** the widgets of each view */
    vector<CSailViewerPanel *> panel;
    /** the tab widget */
    QTabWidget *tabs;

    /** the status bar */
    QStatusBar* statusbar;

    /** the definition of the current hull */
    CHullDef hulldef;
    /** the definition of the current sail */
    CSailDef saildef;
    
    /** the sail */
    CPanelGroup sail;
    /** the developped sail */
    CPanelGroup flatsail;
    /** the display version of the flat sail */
    CPanelGroup dispsail;
    /** the current filename */
    QString filename;

    // menus and action
    /** the File menu */
    QMenu *menuFile;
    /** the Print submenu */
    QMenu *menuPrint;
    /** the Export 3d submenu */
    QMenu *menuExport3d;
    /** the Export flat submenu */
    QMenu *menuExportFlat;
    /** the View menu */
    QMenu *menuView;

    /** the Most Recently Used files menu */
    QMenu *menuRecent;

    /** open an existing sail definition */
    QAction *actionOpen;
    /** sail the sail definition to a file */
    QAction *actionSave;
    /** prompt for a filename then write the sail definition to a file */
    QAction *actionSaveAs;
    /** export 3D sail to DXF */
    QAction *actionExport3dDXF;
    /** export 3D sail to text */
    QAction *actionExport3dTXT;
    /** export 3D sail to XML */
    QAction *actionExport3dXML;
    /** export flat sail to Carlson plotter format */
    QAction *actionExportFlatCarlson;
    /** export flat sail to hand format */
    QAction *actionExportFlatHand;
    /** export flat sail to DXF format */
    QAction *actionExportFlatDXF;
    /** export flat sail to DXF blocks format */
    QAction *actionExportFlatDXFBlocks;
    /** export flat sail to TXT format */
    QAction *actionExportFlatTXT;
    /** export flat sail to XML format */
    QAction *actionExportFlatXML;
    /** print sail data */
    QAction *actionPrintData;
    /** print sail development */
    QAction *actionPrintDev;
    /** print sail drawing */
    QAction *actionPrintDwg;
    /** view sail definition */
    QAction *actionViewDef;
    /** view sail mould */
    QAction *actionViewMould;
    /** view sail definition */
    QAction *actionViewPatch;
    /** quit */
    QAction *actionQuit;
};

#endif
