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

#ifndef RIGDEF_H
#define RIGDEF_H

#include <string>
#include <geocpp/geocpp.h>


/** This class holds the parameters that define a rig
 *  and the default parameters.
 *
 * @ingroup SailCpp
 * @see CRigWorker
 */
class CRigDef
{
public:
    CRigDef();

    /** rig ID name */
    std::string rigID;
    /** fore triangle hoist*/
    real foreI;
    /** fore triangle base*/
    real foreJ;

    ////// mast /////
    /** mast height */
    real MHeight;
    /** mast cord */
    real MCord;
    /** mast width */
    real MWidth;
    /** mast rake */
    real MRakeM;
    /** mast rake angle in degree */
    real MRakeD;
    /** mast base distance to stem */
    real MBase;
    /** mast round */
    real MRnd;
    /** mast round position */
    int MRndPos;

    /** Boom height above deck */
    real BAD;
    /** mainsail tack */
    CVector3d MStack;
    /** mainsail head height above deck */
    real HAD;
    /** mainsail head point */
    CVector3d MShead;

    ////// spreaders and shrouds /////
    /** cap shroud height */
    real CSH;
    /** cap shroud base */
    real CSB;
    /** lower shroud base */
    real LSB;
    /** number of spreaders */
    unsigned int SPNB;
    /** spreader height */
    real SPH[4];
    /** spreader width */
    real SPW[4];
};

#endif
