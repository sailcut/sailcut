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

#include "formboat.h"
#include "sailcutqt.h"
#include "sailviewer-panel.h"
#include "boatdef-panel.h"
#include "hullworker.h"
#include "sailworker.h"
#include "rigworker.h"
#include "sailwriter-xml.h"

#include <QLayout>
#include <QMenuBar>
#include <QFileInfo>

/**
 * The constructor.
 *
 * @param myPrefs the user preferences
 * @param parent the parent widget
 */
CFormBoat::CFormBoat(CPrefs *myPrefs, QWidget *parent)
        : CFormDocumentTmpl<CBoatDef, CBoatDefXmlWriter>(myPrefs, parent)
{
    // create main widget
    setupMainWidget();
    
    // create menu bar
    setupMenuBar();

    // set language
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormBoat::languageChange()
{
    setWindowTitle( tr("boat") );

    /* File menu */
    menuAdd->setTitle( tr("&Add") );
    actionAddSailDef->setText( tr("sail") );
    actionAddHullDef->setText( tr("hull") );
    actionAddRigDef->setText( tr("rig") );
    actionAddPanelGroup->setText( tr("3D panels") );

    defpanel->languageChange();
    viewer->languageChange();
}


/**
 * We got a new boat definition, update widgets.
 *
 * @param newdef The new boat definition
 */
void CFormBoat::setDef(const CBoatDef &newdef)
{
    def = newdef;
    viewer->setObject(def.makePanelGroup());
    defpanel->setDef(def);
}


/**
 * Creates the main widget
 */
void CFormBoat::setupMainWidget()
{
    viewer = new CSailViewerPanel(this, WIREFRAME, true, false);
    defpanel = new CBoatDefPanel(this);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(viewer, 0, 0);
    layout->setRowStretch(0, 2);
    layout->addWidget(defpanel, 1, 0);
    layout->setRowStretch(1, 1);
    
    connect(defpanel, SIGNAL(signalUpdate(const CBoatDef& )), this, SLOT(slotUpdate(const CBoatDef& )));
}


/**
 * Creates the menu bar
 */
void CFormBoat::setupMenuBar()
{
    menuAdd = new QMenu(this);
    actionAddSailDef = menuAdd->addAction( "", this, SLOT( slotAddSailDef() ) );
    actionAddHullDef = menuAdd->addAction( "", this, SLOT( slotAddHullDef() ) );
    actionAddRigDef = menuAdd->addAction( "", this, SLOT( slotAddRigDef() ) );
    actionAddPanelGroup = menuAdd->addAction("", this, SLOT( slotAddPanelGroup() ) );
    extraFileMenus.push_back(menuAdd);
}


/**
 * The file menu's "Add->Panels" item was clicked.
 */
void CFormBoat::slotAddPanelGroup()
{
    CBoatElement element;
    QString newname = CPanelGroupXmlWriter().readDialog((CPanelGroup&)element,"");

    if (!newname.isNull())
    {
        element.type = PANELGROUP;
        element.filename = newname;
        def.element.push_back(element);
        setDef(def);
    }

}


/**
 * The file menu's "Add->Hull definition" item was clicked.
 */
void CFormBoat::slotAddHullDef()
{
    CHullDef hulldef;
    QString newname = CHullDefXmlWriter().readDialog(hulldef,"");

    if (!newname.isNull())
    {
        CBoatElement element;
        (CPanelGroup&)element = CHullWorker(hulldef).makeHull();
        element.type = HULLDEF;
        element.filename = newname;
        def.element.push_back(element);
        setDef(def);
    }

}


/**
 * The file menu's "Add->Sail definition" item was clicked.
 */
void CFormBoat::slotAddSailDef()
{
    CSailDef saildef;
    QString newname = CSailDefXmlWriter().readDialog(saildef,"");

    if (!newname.isNull())
    {
        CBoatElement element;
        (CPanelGroup&)element = CSailWorker(saildef).makeSail();
        element.type = SAILDEF;
        element.filename = newname;
        def.element.push_back(element);
        setDef(def);
    }

}


/**
 * The file menu's "Add->Rig definition" item was clicked.
 */
void CFormBoat::slotAddRigDef()
{
    CRigDef rigdef;
    QString newname = CRigDefXmlWriter().readDialog(rigdef,"");

    if (!newname.isNull())
    {
        CBoatElement element;
        (CPanelGroup&)element = CRigWorker(rigdef).makeRig();
        element.type = RIGDEF;
        element.filename = newname;
        def.element.push_back(element);
        setDef(def);
    }

}


/**
 * The boat definition was modified by the user.
 *
 * @param newdef The new boat definition
 */
void CFormBoat::slotUpdate(const CBoatDef& newdef)
{
    def = newdef;
    viewer->setObject(def.makePanelGroup());
}


