/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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
#include "sailtreemodel.h"

#include <QLayout>
#include <QSplitter>
#include <QTreeView>


/**
 * Constructs a window to display a collection of panels.
 *
 * @param myPrefs the user preferences
 * @param parent the parent widget
 */
CFormPanelGroup::CFormPanelGroup(CPrefs *myPrefs, QWidget *parent)
        : CFormDocumentTmpl<CPanelGroup, CPanelGroupXmlWriter> (myPrefs, parent)
{
    setMinimumSize( QSize( 400, 400 ) );

    // create main widget
    setupMainWidget();

    // set language
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormPanelGroup::languageChange()
{
    setWindowTitle( tr("panels") );
    tabs->languageChange();
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormPanelGroup::setDef(const CPanelGroup& newdef)
{
    def = newdef;
    tabs->setObject(def);
    treeview->setModel(new CSailTreeModel(def, "panelgroup"));
#if QT_VERSION >= 0x040200
    treeview->expandAll();
#endif
//    treeview->resizeColumnToContents(0);
}


/**
 * Creates the main widget
 */
void CFormPanelGroup::setupMainWidget()
{
    tabs = new CSailViewerTabs(this);
    treeview = new QTreeView(this);

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(tabs);
    splitter->addWidget(treeview);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(splitter);
}

