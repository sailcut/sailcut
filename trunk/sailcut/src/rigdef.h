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

#ifndef RIGDEF_H
#define RIGDEF_H

#include "panelgroup.h"

typedef enum { SAILDEF, HULLDEF, PANELGROUP } enumRigSailType;

/**
 * A rig sail. This class extends a regular sail with information
 *  about how to integrate it into a rig.
 *
 * @see CSail
 * @see CRigDef
 */
class CRigSail : public CPanelGroup
{
public:
    /** the type of file this sail was read from (sail definition or 3D sail) */
    enumRigSailType type;
    /** the name of the file this sail was read from */
    QString filename;
    /** the origin of the sail */
    CPoint3d origin;
};


/**
 * A rig definition, which consists of a collection of rig sails.
 *
 * @see CRigSail
 */
class CRigDef
{
public:
    CRigDef();

    CPanelGroup makeViewSail() const;

    /** the collection of sails that makes up the rig */
    vector<CRigSail> rigsail;
};

#endif

