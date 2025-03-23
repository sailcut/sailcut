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

#include "boatdef.h"

/** The constructor.
 */
CBoatDef::CBoatDef()
{}


/** Produce the combined panel group made up of all the panels
 */
CPanelGroup CBoatDef::makePanelGroup() const
{
    CPanelGroup output;

    for (unsigned int i = 0; i < size(); i++) {
        CMatrix4x4 matrix;
        matrix.translate(at(i).origin);
        output.child.push_back(at(i).transformed(matrix));
    }

    return output;
}
