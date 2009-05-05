/*
 * Copyright (C) 1993-2009 Robert & Jeremy Laine
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

#ifndef FORMPANELGROUP_H
#define FORMPANELGROUP_H

#include "formdocument.h"
#include "sailwriter-xml.h"

// forward definitions
class QTreeView;

/** Dialog holding a sail.
 */
class CFormPanelGroup : public CFormDocumentTmpl<CPanelGroup, CPanelGroupXmlWriter>
{
    Q_OBJECT

public:
    CFormPanelGroup(CPrefs *myPrefs, QWidget *parent);

protected:
    T_KEYPRESS
    void setDef(const CPanelGroup& newdef);
    void setupMainWidget();

protected slots:
    virtual void languageChange();

protected:
    /** the tree view */
    QTreeView *treeview;
};

#endif
