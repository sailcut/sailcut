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

#ifndef SAILDEF_H
#define SAILDEF_H

#include <QString>

#include "sailmould.h"


// enumerated types
enum enumSailType { MAINSAIL, JIB, WING };
enum enumSailCut { CROSS, TWIST, HORIZONTAL, VERTICAL, RADIAL, MITRE };


/** This class holds the parameters that define a sail
 *  and the default parameters
 *
 *  @see CFormSailDef for data input and checking, CSailWorker
 */
class CSailDef
{
public:
    CSailDef();

    CSailDef & operator=( const CSailDef & );

    // member variables
    /** sail ID name */
    QString sailID;
    /** type of cut */
    enumSailCut sailCut;
    /** type of sail */
    enumSailType sailType;
    /** boat deck Length */
    real LOA;
    /** Fore triangle */
    real foreI, foreJ;
    /** tack distance from stem */
    real tackX;
    /** tack height above deck */ 
    real tackY;
    /** straight line luff length */
    real luffL;
    /** rake of mast in mm */
    real rake;
    /** gaff angle from luff line in degree */
    real gaffDeg;
    /** straight line gaff length */
    real gaffL;
    /** straight line foot length */
    real footL;
    /** straight line leech length */
    real leechL;
    /** value of round of sides */
    real luffR,  gaffR,  leechR,  footR;
    /** position of round of sides in percent */
    int  luffRP, gaffRP, leechRP, footRP;
    /** cloth width */
    real clothW;
    /** seams width */
    real seamW, leechHemW, hemsW;

    /** sail twist */
    real twistDeg;
    /** sail sheeting angle */
    real sheetDeg;

    /** wing diedral angle */
    real dihedralDeg;

    /** number of radial sections */
    unsigned int nbSections;
    /** number of radial gores */
    unsigned int nbGores;
    /** number of luff gores */
    unsigned int nbLuffGores;

    /** sail mould */
    CSailMould mould;
};

#endif
