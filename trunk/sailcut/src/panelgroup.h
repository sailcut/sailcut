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

#ifndef PANELGROUP_H
#define PANELGROUP_H

#include "panel.h"

enum enumBoatObjType { SAIL, HULL };

/** This class holds a collection of panels, for instance to represent
 *  a 3D or a developed sail.
 */
class CPanelGroup
{
public:
    CPanelGroup( unsigned int = 0 );
    CPanelGroup( const CPanelGroup& );
    CPanelGroup( const CPanel& );

    /** the panels in this group */
    vector<CPanel> panel;

    /** title of this panel group */
    QString title;

    /** type of boat object */
    enumBoatObjType type;

    // member functions
public:
    /** rectangle containing all panels */
    CRect3d boundingRect() const;

    /** function to place the labels on a displayed panel group */
    void placeLabels();
    /** function to place the labels on a plotted panel group */
    void plotLabels();

    /** function for rotating all panels */
    CPanelGroup rotate( const CPoint3d &, const CMatrix & ) const;

    // operators
    CPanelGroup& operator=( const CPanelGroup &);
    CPanelGroup operator+ (const CVector3d &) const;
    friend ostream& operator<< (ostream &, const CPanelGroup &);
};


// global functions
ostream& operator<< (ostream &, const CPanelGroup &);

#endif
