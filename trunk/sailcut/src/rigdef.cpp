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

#include "rigdef.h"


/** Constructs a CRigDef object.
 */
CRigDef::CRigDef()
{
    // default values for member variables are set here
    rigID = "Test rig 1";
    foreI  = 5500;    // height of fore triangle
    foreJ  = 2000;    // base of fore triangle
    MHeight = 8000;   // mast height
    MCord = 140;
    MWidth = 70;
    MRakeM = 200;
    MRakeD = 0;
    CSH = 7000;   // cap shroud height
    CSB = 1100;
    LSB = 900;
    SPNB = 1;     // number of spreaders
    SPH1 = 3500;
    SPW1 = 1000;
    SPH2 = 0;
    SPW2 = 0;
    SPH3 = 0;
    SPW3 = 0;
    
}


/** Assignment operator.
 * 
 * @param copy the rig definition to copy from
 * @return CRigDef&
 */
CRigDef & CRigDef::operator=( const CRigDef &copy )
{
    if (&copy == this)
        return *this;
    // else
    rigID = copy.rigID;
    foreI  = copy.foreI;    // height of fore triangle
    foreJ  = copy.foreJ;    // base of fore triangle
    MHeight = copy.MHeight; // mast height
    MCord = copy.MCord;
    MWidth = copy.MWidth;
    MRakeM = copy.MRakeM;
    MRakeD = copy.MRakeD;
    CSH = copy.CSH;  // outer shroud height
    CSB = copy.CSB;
    LSB = copy.LSB;
    SPNB = copy.SPNB; // number of spreaders
    SPH1 = copy.SPH1;
    SPW1 = copy.SPW1;
    SPH2 = copy.SPH2;
    SPW2 = copy.SPW2;
    SPH3 = copy.SPH3;
    SPW3 = copy.SPW3;
    
    return *this;
}


