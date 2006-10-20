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

#ifndef FORMRIG_H
#define FORMRIG_H

#include "formdocument.h"
#include "rigdef.h"

class QAction;
class QMenu;
class CSailViewerPanel;
class CRigDefPanel;
class CPrefs;


/** A form allowing the user to assemble several sails into a rig.
 */
class CFormRig : public CFormDocument
{
    Q_OBJECT

public:
    // construction, destruction
    CFormRig(CPrefs *myPrefs);

    //virtual bool open();
    virtual bool save();
    virtual bool saveAs();

protected:
    void closeEvent( QCloseEvent * e);
    void setRigDef(const CRigDef &newdef);
    void setupMainWidget();
    void setupMenuBar();

protected slots:
    virtual void languageChange();
    virtual void slotAddSail();
    virtual void slotAddSailDef();
    virtual void slotNew();
    virtual void slotOpen();
    virtual void slotUpdate(const CRigDef& newdef);

protected:
    /** An area to view the rig. */
    CSailViewerPanel *viewer;
    /** A panel with one tab per sail in the current rig.  */
    CRigDefPanel *defpanel;
    /** The rig definition. */
    CRigDef rigdef;

    /** The File menu. */
    QMenu *menuFile;
    /** The Add menu. */
    QMenu *menuAdd;

    /** create a new rig def */
    QAction *actionNew;
    /** add a sail definition */
    QAction *actionAddSailDef;
    /** add a sail */
    QAction *actionAddSail;
    /** read a rig from file */
    QAction *actionOpen;

};

#endif
