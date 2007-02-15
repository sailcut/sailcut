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

#include "formrig.h"
#include "formrigdef.h"
#include "rigworker.h"

#include <QLayout>

/**
 * Constructs a window to display a rig.
 *
 * @param myPrefs the user preferences
 * @param parent the parent widget
 */
CFormRig::CFormRig(CPrefs *myPrefs, QWidget *parent)
        : CFormDocumentTmpl<CRigDef, CRigDefXmlWriter> (myPrefs, parent)
{
    setMinimumSize( QSize( 300, 220 ) );

    // create menu bar
    setupMenuBar();

    // create main widget
    QGridLayout *layout = new QGridLayout(this);
    tabs = new CSailViewerTabs(this);
    layout->addWidget(tabs);

    // set language
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormRig::languageChange()
{
    setWindowTitle( tr("rig") );
    actionViewDef->setText( tr("&Dimensions") );
    tabs->languageChange();
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormRig::setDef(const CRigDef& newdef)
{
    def = newdef;
    tabs->setObject(CRigWorker(def).makeRig());
}


/**
 * Creates the menu bar
 */
void CFormRig::setupMenuBar()
{
    // view actions
    actionViewDef = new QAction(this);
    connect( actionViewDef, SIGNAL( triggered() ), this, SLOT( slotDef() ) );
    extraViewActions.push_back(actionViewDef);
}


/**
 * Displays the CFormRigDef rig definition dialog.
 */
void CFormRig::slotDef()
{
    CRigDef defcopy = def;

    if ( CFormRigDef(this, &defcopy).exec() )
    {
        // we returned from the dialog with an 'OK',
        setDef(defcopy);
    }
}

