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

#include "formpanelgroup.h"
#include "sailviewer-panel.h"
#include "sailtreemodel.h"

#include <QLayout>
#include <QTreeView>


/**
 * Constructs a window to display a collection of panels.
 *
 * @param prefs the user preferences
 * @param parent the parent widget
 */
CFormPanelGroup::CFormPanelGroup(CPrefs *myPrefs, QWidget *parent)
        : CFormDocumentTmpl<CPanelGroup, CPanelGroupXmlWriter> (myPrefs, parent)
{
    setMinimumSize( QSize( 300, 220 ) );

    // create main widget
    setupMainWidget();

    // set language
    languageChange();
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormPanelGroup::languageChange()
{
    setWindowTitle( "Panels" );
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormPanelGroup::setDef(const CPanelGroup& newdef)
{
    def = newdef;
    viewer->setObject(def);

    CSailTreeModel *model = new CSailTreeModel(def, "panelgroup");
    treeview->setModel(model);
}


/**
 * Creates the main widget
 */
void CFormPanelGroup::setupMainWidget()
{
    QGridLayout *layout = new QGridLayout(this);
    
    viewer = new CSailViewerPanel(0, WIREFRAME, true);
    layout->addWidget(viewer);

    treeview = new QTreeView(this);
    layout->addWidget(treeview);
}



