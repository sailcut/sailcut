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

#ifndef BOATDEF_H
#define BOATDEF_H

#include "panelgroup.h"

typedef enum { SAILDEF, HULLDEF, RIGDEF, PANELGROUP } enumBoatElementType;

/**
 * A boat element. This class extends a CPanelGroup with information
 *  about how to integrate it into a boat.
 *
 * @see CPanelGroup
 * @see CBoatDef
 */
class CBoatElement : public CPanelGroup
{
public:
    /** the type of file this element was read from (sail, hull definition or 3D panels) */
    enumBoatElementType type;
    /** the name of the file this element was read from */
    string filename;
    /** the origin of the element */
    CPoint3d origin;
};


/**
 * A boat definition, which consists of a collection of boat elements.
 *
 * @see CBoatElement
 */
class CBoatDef : public vector<CBoatElement>
{
public:
    CBoatDef();
    CPanelGroup makePanelGroup() const;
};

#endif

