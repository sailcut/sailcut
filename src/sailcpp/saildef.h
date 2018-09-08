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

#ifndef SAILDEF_H
#define SAILDEF_H

#include <string>

#include "sailmould.h"


// enumerated types
enum enumSailType { MAINSAIL, JIB, WING };
enum enumSailCut { CROSS, TWIST, HORIZONTAL, VERTICAL, RADIAL, MITRE , MITRE2};


/** The CSailDef class holds the parameters that define a sail
 *  and its default values.
 *
 * @ingroup SailCpp
 * @see CFormSailDef for data input and checking
 * @see CSailWorker
 */
class CSailDef
{
public:
    CSailDef();

    CSailDef & operator=( const CSailDef & );

    // member variables
    /** The Sail ID name */
    string sailID;
    /** The type of cut layout */
    enumSailCut sailCut;
    /** The type of sail */
    enumSailType sailType;
    /** The boat deck Length in mm */
    real LOA;
    /** The Fore triangle hoist in mm */
    real foreI;
    /** The Fore triangle base in mm */
    real foreJ;
    /** The distance from tack to stem */
    real tackX;
    /** The Tack height above deck in mm */
    real tackY;
    /** The straight line luff length in mm */
    real luffL;
    /** The rake of Luff in mm */
    real rake;
    /** The gaff angle from Luff line in degree */
    real gaffDeg;
    /** The straight line Gaff length in mm */
    real gaffL;
    /** The straight line Foot length in mm */
    real footL;
    /** The straight line Leech length in mm */
    real leechL;
    /** The value of round of sides */
    real luffR,  gaffR,  leechR,  footR;
    /** The position of round of sides in percent of its length*/
    int  luffRP, gaffRP, leechRP, footRP;
    /** The cloth width */
    real clothW;
    /** The seam width between panels in mm */
    real seamW;
    /** The leech seam width in mm */
    real leechHemW;
    /** The foot seam width in mm */
    real footHemW;
    /** The other seams width in mm */
    real hemsW;
    /** The Inner guideline width in mm */
    real innerW;

    /** The Sail twist between foot and head in degrees */
    real twistDeg;
    /** The sail sheeting angle in degrees */
    real sheetDeg;

    /** The Wing diedral angle in degrees */
    real dihedralDeg;

    /** The number of radial sections */
    unsigned int nbSections;
    /** The number of radial head gores */
    unsigned int nbGores;
    /** The number of luff gores */
    unsigned int nbLuffGores;

    /** The sail mould */
    CSailMould mould;
};

#endif
