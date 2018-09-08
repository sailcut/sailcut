/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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

#include "saildef.h"


/**************************************************************************

                       construction / destruction

**************************************************************************/

/** Constructs a CSailDef object from the default sail parameters.
 *
 */
CSailDef::CSailDef()
{
    // default values for member variables are set here
    sailType = MAINSAIL;
    sailCut = CROSS;
    sailID = "Test sail 1";

    LOA    = 7000;   // length of hull in millimetre
    foreJ  = 2000;    // base of fore triangle
    foreI  = 6500;    // height of fore triangle

    tackX  = 1972;    // longitudinal distance of main sail tack from stem in millimetre
    tackY  = 750;     // height of sail tack above stem in millimetre
    luffL  = 6250;    // in millimetre
    rake   = 161;     // in millimetre
    gaffDeg= 45;     // in degree
    gaffL  = 2500;   // in millimetre
    leechL = 8220;   // in millimetre
    footL  = 4100;   // in millimetre

    luffR  = 67;     // in millimetre // NOTE: Positive value for mast bend Negative for headstay sag
    luffRP = 50;      // in percent of luff length
    gaffR  = 150;    // in millimetre
    gaffRP = 50;     // in percent
    leechR = -120;  // in millimetre // NOTE: Negative value for hollow leech
    leechRP= 55;    // in percent
    footR  = 150;   // in millimetre // NOTE: Positive value for foot roach
    footRP = 50;    // in percent

    clothW = 900;        // in millimetre
    seamW = 13;         // in millimetre
    leechHemW = 40;  // in millimetre
    footHemW = 30;
    hemsW = 10;        // in millimetre

    twistDeg = 18;      // in degree
    sheetDeg = 0;

    dihedralDeg = 168;     // in degree

    nbSections = 5;      // radial sections
    nbGores = 5;         // radial gores
    nbLuffGores = 2;   // luff gores
}



/**************************************************************************

                               operators

**************************************************************************/


/** Assignment operator
 * @param copy the sail definition to copy from
 * @return CSailDef&
 */
CSailDef & CSailDef::operator=( const CSailDef &copy )
{
    if (&copy == this)
        return *this;
    // else
    sailID = copy.sailID;
    sailCut = copy.sailCut;
    sailType = copy.sailType;

    footL = copy.footL;
    footR = copy.footR;
    footRP = copy.footRP;
    foreI = copy.foreI;
    foreJ = copy.foreJ;
    gaffDeg = copy.gaffDeg;
    gaffL = copy.gaffL;
    gaffR = copy.gaffR;
    gaffRP = copy.gaffRP;
    leechL = copy.leechL;
    leechR = copy.leechR;
    leechRP = copy.leechRP;
    LOA = copy.LOA;
    luffL = copy.luffL;
    luffR = copy.luffR;
    luffRP = copy.luffRP;
    mould = copy.mould;
    rake = copy.rake;

    clothW = copy.clothW;
    seamW = copy.seamW;
    leechHemW = copy.leechHemW;
    footHemW = copy.footHemW;
    hemsW = copy.hemsW;

    tackX = copy.tackX;
    tackY = copy.tackY;

    twistDeg = copy.twistDeg;
    sheetDeg = copy.sheetDeg;

    dihedralDeg = copy.dihedralDeg;

    nbSections = copy.nbSections;
    nbGores = copy.nbGores;
    nbLuffGores = copy.nbLuffGores;

    return *this;
}

