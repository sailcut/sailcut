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
