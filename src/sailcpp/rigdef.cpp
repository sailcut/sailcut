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

#include <cmath>

#include "rigdef.h"


/** Constructs a CRigDef object from the default rig parameters.
 *
 */
CRigDef::CRigDef()
{
    // default values for member variables are set here
    rigID = "Test rig 1";
    foreI  = 6500;    // height of fore triangle
    foreJ  = 2000;    // base of fore triangle
    MHeight = 8000;   // mast height
    MCord = 140;
    MWidth = 70;
    MRakeM = 220;
    MRakeD = radiansToDegrees(atan2(MRakeM, MHeight));
    MBase = foreJ - MRakeM * (foreI/MHeight);
    MRnd = 110;
    MRndPos = 50;

    CSH = 7000;     // cap shroud height
    CSB = 500;
    LSB = 400;
    SPNB = 3;       // number of spreaders
    SPH[0] = 0;
    SPW[0] = CSB;
    SPH[1] = 1800;
    SPW[1] = CSB;
    SPH[2] = 3600;
    SPW[2] = CSB -50;
    SPH[3] = 5400;
    SPW[3] = CSB -200;

    BAD = 750;      // height of boom
    //MStack = CVector3d(2100, 750 , 0);
    HAD = 7000;
    //MShead = CVector3d(2200, 7000, 0);
}
