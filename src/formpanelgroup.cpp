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

#include "formpanelgroup.h"
#include "sailtreemodel.h"

#include <QApplication>
#include <QLayout>
#include <QSplitter>
#include <QTreeView>


/**
 * Constructs a window to display a collection of panels.
 *
 * @param parent the parent widget
 */
CFormPanelGroup::CFormPanelGroup(QWidget *parent)
    : CFormMain(parent)
{
    // create main widget
    setupMainWidget();

    // set language
    connect(qApp, SIGNAL(languageChanged()),
            this, SLOT(languageChange()));
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
}


bool CFormPanelGroup::read(const QString &filename)
{
    try {
        setDef(writer.read(filename));
        return true;
    } catch (read_error const&) {
        return false;
    }
}


bool CFormPanelGroup::write(const QString &filename)
{
    try {
        writer.write(def, filename);
        return true;
    } catch (write_error const&) {
        return false;
    }
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

    setCentralWidget(splitter);
}

