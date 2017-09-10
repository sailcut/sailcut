/*
 * Copyright (C) 1993-2017 Robert & Jeremy Laine
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

#include "hulldef.h"


/** Constructs a CHullDef object from the default hull parameters
 *
 */
CHullDef::CHullDef()
{
    // default values for hull member variables are set here
    hullID = "Test hull 1";
    DLOA    = 7500;    // Deck length in millimetre
    DfwdHeight = 1000; // Deck height forward
    DaftHeight = 850;  // Deck height aft
    DSlopeA = 6;       // Deck side slope in degree from horizontal
    DBW = 2500;        // Deck max beam width
    DBWPos = 50;       // percent of deck length
    DaftW = 2000;      // Deck aft width
    StemA = 65;        // Stem inclination in degree from horizontal
    TransomA = 80;     // Transom inclination

    BLWL    = 7000;    // Bottom length in millimetre
    BfwdHeight = 100;  // Bottom chine forward height
    BaftHeight = 200;  // Bottom chine aft height
    BSlopeA = 12;      // Bottom chine plane side slope in degree from horizontal
    BBW = 2500;        // Bottom max beam width
    BBWPos = 50;       // Bottom chine max width position percent of deck length
    BaftW = 2000;      // Bottom aft width
    BDeadriseA = 12;   // Bottom plank dead rise from horizontal
    BSweepA = 30;      // Bottom plank sweep angle in degree
    BfwdShape = 2;     // forward coefficient power of X
    BaftShape = 3;     // aft coefficient power of X

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

    BLWL = copy.BLWL;
    BfwdHeight = copy.BfwdHeight;
    BaftHeight = copy.BaftHeight;
    BSlopeA = copy.BSlopeA;
    BBW = copy.BBW;
    BBWPos = copy.BBWPos;
    BaftW = copy.BaftW;

    BDeadriseA = copy.BDeadriseA;
    BSweepA = copy.BSweepA;
    BfwdShape = copy.BfwdShape;
    BaftShape = copy.BaftShape;

    NBPlank = copy.NBPlank;
    AutoPlank = copy.AutoPlank;
    TopPlankA = copy.TopPlankA;
    LowPlankA = copy.LowPlankA;

    return *this;
}


