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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "formhull.h"
#include "formhulldef.h"

#include "hullworker.h"
#include "sailviewer-panel.h"

#include <QLayout>
#include <QTabWidget>


/**
 * Constructs a window to display a hull.
 *
 * @param myPrefs the user preferences
 * @param parent the parent widget
 */
CFormHull::CFormHull(CPrefs *myPrefs, QWidget *parent)
        : CFormDocumentTmpl<CHullDef, CHullDefXmlWriter> (myPrefs, parent)
{
    setMinimumSize( QSize( 300, 220 ) );

    // create menu bar
    setupMenuBar();

    // create main widget
    setupMainWidget();

    // set language
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * We received a keypress, we pass it down to the visible tab.
 */
void CFormHull::keyPressEvent ( QKeyEvent * e )
{
    panel[tabs->currentIndex()]->keyPressEvent(e);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormHull::languageChange()
{
    int tabidx = 0;
    setWindowTitle( tr("hull") );

    // view menu
    actionViewDef->setText( tr("&Dimensions") );

    // tabs
    for (unsigned int i = 0; i < panel.size(); i++)
        panel[i]->languageChange();
#ifdef HAVE_QTOPENGL
    tabs->setTabText(tabidx++, tr("shaded view"));
#endif
    tabs->setTabText(tabidx++, tr("wireframe view"));
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormHull::setDef(const CHullDef& newdef)
{
    def = newdef;
    CPanelGroup obj_3d = CHullWorker(def).makeHull();
    for (unsigned int i = 0; i < panel.size(); i++)
        panel[i]->setObject(obj_3d);
}


/**
 * Creates the menu bar
 */
void CFormHull::setupMenuBar()
{
    // view actions
    actionViewDef = new QAction(this);
    connect( actionViewDef, SIGNAL( triggered() ), this, SLOT( slotDef() ) );
    extraViewActions.push_back(actionViewDef);
}


/**
 * Creates the main widget
 */
void CFormHull::setupMainWidget()
{
    // create viewers
    CSailViewerPanel *tmp;
#ifdef HAVE_QTOPENGL
    tmp = new CSailViewerPanel(0, SHADED, true);
    panel.push_back(tmp);
#endif
    tmp = new CSailViewerPanel(0, WIREFRAME, true);
    panel.push_back(tmp);
   
    // create tabs 
    QGridLayout *layout = new QGridLayout(this);
    tabs = new QTabWidget(this);
    layout->addWidget(tabs);
    for (unsigned int i = 0 ; i < panel.size(); i++)
        tabs->addTab(panel[i],"");
}


/**
 * Displays the sail CFormHullDef sail definition dialog.
 */
void CFormHull::slotDef()
{
    CHullDef defcopy = def;

    if ( CFormHullDef(this, &defcopy).exec() )
    {
        // we returned from the dialog with an 'OK',
        setDef(defcopy);
    }
}

