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

#include "hulldef.h"


/** Constructs a CHullDef object.
 */
CHullDef::CHullDef()
{
    // default values for member variables are set here
    hullID = "Test hull 1";
    LOA    = 7000;  // length of hull in millimetre
}


/** Assignment operator.
 * 
 * @param copy the hull definition to copy from
 * @return CHullDef&
 */
CHullDef & CHullDef::operator=( const CHullDef &copy )
{
    if (&copy == this)
    {
        CException e("CHullDef::operator= : self-assignment!");
        throw(e);
    }
    hullID = copy.hullID;
    LOA = copy.LOA;
    return *this;
}


