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

#ifndef FORMBOAT_H
#define FORMBOAT_H

#include "formdocument.h"
#include "sailwriter-xml.h"

class CSailViewerPanel;
class CBoatDefPanel;


/** A form allowing the user to assemble several elements into a boat.
 */
class CFormBoat : public CFormDocumentTmpl<CBoatDef, CBoatDefXmlWriter>
{
    Q_OBJECT

public:
    CFormBoat(CPrefs *myPrefs, QWidget *parent);

protected:
    void setDef(const CBoatDef &newdef);
    void setupMainWidget();
    void setupMenuBar();

protected slots:
    virtual void languageChange();
    virtual void slotAddHullDef();
    virtual void slotAddPanelGroup();
    virtual void slotAddSailDef();
    virtual void slotAddRigDef();
    virtual void slotUpdate(const CBoatDef& newdef);

protected:
    /** An area to view the boat. */
    CSailViewerPanel *viewer;
    /** A panel with one tab per element in the current boat.  */
    CBoatDefPanel *defpanel;

    /** The Add menu. */
    QMenu *menuAdd;

    /** add a sail definition */
    QAction *actionAddHullDef;
    /** add a collection of panels */
    QAction *actionAddPanelGroup;
    /** add a sail definition */
    QAction *actionAddSailDef;
    /** add a rig definition */
    QAction *actionAddRigDef;

};

#endif
