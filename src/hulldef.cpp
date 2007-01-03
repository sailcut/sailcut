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
    DLOA    = 7000;  // length of hull in millimetre
    DfwdHeight = 1000;
    DmidHeight = 800;
    DaftHeight = 850;
    DBW = 2500;
    DBWPos = 50;
    DaftW = 2000;
    DfwdShape = 2;
    DaftShape = 3;
    StemA = 80;
    
    BfwdHeight = 100;
    BmidHeight = 0;
    BaftHeight = 200;
    BdeadriseA = 12;
    BsweepA = 45;
    
    NBPlank = 3;
    AutoPlank = true;
    TopPlankA = 80;
    LowPlankA = 30;
    
    
}


/** Assignment operator.
 * 
 * @param copy the hull definition to copy from
 * @return CHullDef&
 */
CHullDef & CHullDef::operator=( const CHullDef &copy )
{
    if (&copy == this)
        return *this;
    // else new data to be copied
    hullID = copy.hullID;
    DLOA = copy.DLOA;
    DfwdHeight = copy.DfwdHeight;
    DmidHeight = copy.DmidHeight;
    DaftHeight = copy.DaftHeight;
    DBW = copy.DBW;
    DBWPos = copy.DBWPos;
    DaftW = copy.DaftW;
    DfwdShape = copy.DfwdShape;
    DaftShape = copy.DaftShape;
    StemA = copy.StemA;
    
    BfwdHeight = copy.BfwdHeight;
    BmidHeight = copy.BmidHeight;
    BaftHeight = copy.BaftHeight;
    BdeadriseA = copy.BdeadriseA;
    BsweepA = copy.BsweepA;
    
    NBPlank = copy.NBPlank;
    AutoPlank = copy.AutoPlank;
    TopPlankA = copy.TopPlankA;
    LowPlankA = copy.LowPlankA;
    
    return *this;
}


