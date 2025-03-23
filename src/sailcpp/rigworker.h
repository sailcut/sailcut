/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef RIGWORKER_H
#define RIGWORKER_H

#include "panelgroup.h"
#include "rigdef.h"


/** The CRigWorker class does all the rig-related calculations.
 *  It is used to create the rig from its definition.
 *
 * @ingroup SailCpp
 * @see CRigDef, CPanelGroup
 */
class CRigWorker : public CRigDef
{
public:
    CRigWorker(const CRigDef &d);

    CPanelGroup makeRig() const;

    // point of mast center at given height
    CPoint3d mastCenter( const real &h ) const;

};

#endif
