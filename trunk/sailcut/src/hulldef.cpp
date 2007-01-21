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
    // default values for hull member variables are set here
    hullID = "Test hull 1";
    DLOA    = 7000;   // length of hull in millimetre
    DfwdHeight = 1000;
    DaftHeight = 850;
    DSlopeA = 6;      // side slope in degree from horizontal
    DBW = 2500;
    DBWPos = 50;      // percent of deck length
    DaftW = 2000;
    StemA = 85;       // in degree from horizontal
    TransomA = 80;
    DfwdShape = 2;    // coefficient power of X
    DaftShape = 3;
    
    BfwdHeight = 100;
    BaftHeight = 200;
    BSlopeA = 24;     // side slope in degree from horizontal
    BdeadriseA = 12;
    BsweepA = 45;
    
    NBPlank = 3;
    AutoPlank = true;
    TopPlankA = 75;   // inclination in degree from horizontal
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
    DaftHeight = copy.DaftHeight;
    DSlopeA = copy.DSlopeA;
    DBW = copy.DBW;
    DBWPos = copy.DBWPos;
    DaftW = copy.DaftW;
    StemA = copy.StemA;
    TransomA = copy.TransomA;
    DfwdShape = copy.DfwdShape;
    DaftShape = copy.DaftShape;
    
    BfwdHeight = copy.BfwdHeight;
    BaftHeight = copy.BaftHeight;
    BSlopeA = copy.BSlopeA;
    BdeadriseA = copy.BdeadriseA;
    BsweepA = copy.BsweepA;
    
    NBPlank = copy.NBPlank;
    AutoPlank = copy.AutoPlank;
    TopPlankA = copy.TopPlankA;
    LowPlankA = copy.LowPlankA;
    
    return *this;
}


