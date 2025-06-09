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

#ifndef PANELGROUP_H
#define PANELGROUP_H

#include "panel.h"

enum enumPanelGroupType { SAIL, RIG, HULL };

/** This class holds a collection of panels, for instance to represent
 *  a 3D or a developed sail.
 *
 * @ingroup SailCpp
 */
class CPanelGroup : public std::vector<CPanel>
{
public:
    CPanelGroup( unsigned int = 0 );
    CPanelGroup( const CPanelGroup& );
    CPanelGroup( const CPanel& );

    /** title of this panel group */
    std::string title;

    /** children of this group */
    std::vector<CPanelGroup> child;

    /** type of boat object */
    enumPanelGroupType type;

    // member functions
public:
    /** rectangle containing all panels */
    CRect3d boundingRect() const;

    /** function to place the labels on a displayed panel group */
    void placeLabels();
    /** function to place the labels on a plotted panel group */
    void plotLabels();

    CPanelGroup transformed(const CMatrix4x4 &m) const;

    // operators
    CPanelGroup& operator=( const CPanelGroup &);
    friend std::ostream& operator<< (std::ostream &, const CPanelGroup &);
};


// global functions
std::ostream& operator<< (std::ostream &, const CPanelGroup &);

#endif
