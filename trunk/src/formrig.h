/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: formrig.h,v 1.11 2006/01/25 21:54:00 jeremy_laine Exp $
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

#ifndef FORMRIG_H
#define FORMRIG_H

#include <QMainWindow>
#include "rigdef.h"

class QAction;
class QMenu;
class CSailViewerPanel;
class CRigDefPanel;
class CPrefs;
class CSailApp;


/** A form allowing the user to assemble several sails into a rig.
 */
class CFormRig : public QMainWindow
{
    Q_OBJECT

public:
    // construction, destruction
    CFormRig(CSailApp *myApp, QWidget *parent = 0);

protected:
    void closeEvent( QCloseEvent * e);
    void makeMenuMru();
    void setRigDef(const CRigDef &newdef);
    void setupMainWidget();
    void setupMenuBar();

protected slots:
    virtual void languageChange();
    virtual void slotAddSail();
    virtual void slotAddSailDef();
    virtual void slotNew();
    virtual void slotOpen();
    virtual void slotOpenRecent();
    virtual void slotSave();
    virtual void slotSaveAs();
    virtual void slotUpdate(const CRigDef& newdef);

protected:
    /** The user's preferences. */
    CPrefs *prefs;
    /** An area to view the rig. */
    CSailViewerPanel *viewer;
    /** A panel with one tab per sail in the current rig.  */
    CRigDefPanel *defpanel;
    /** The rig definition. */
    CRigDef rigdef;
    /** The current filename. */
    QString filename;

    /** The File menu. */
    QMenu *menuFile;
    /** The Add menu. */
    QMenu *menuAdd;
    /** The submenu with the most recently used rig definitions. */
    QMenu *menuRecent;

    /** create a new rig def */
    QAction *actionNew;
    /** add a sail definition */
    QAction *actionAddSailDef;
    /** add a sail */
    QAction *actionAddSail;
    /** read a rig from file */
    QAction *actionOpen;
    /** save rig to file */
    QAction *actionSave;
    /** prompt user for a filename and save rig to file */
    QAction *actionSaveAs;
    /** close the rig viewer */
    QAction *actionClose;

};

#endif
