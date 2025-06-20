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

#ifndef HULLDEF_H
#define HULLDEF_H

#include <string>
#include <geocpp/geocpp.h>


/** This class holds the parameters that define a hull
 *  and their default values.
 *
 * @ingroup SailCpp
 * @see CHullWorker
 */
class CHullDef
{
public:
    CHullDef();

    /** hull ID name */
    std::string hullID;

        //deck
    /** deck Length in millimeter */
    real DLOA;
    /** deck forward height in millimeter */
    real DfwdHeight;
    /** deck aft height in millimeter */
    real DaftHeight;
    /** deck side slope angle in degree from horizontal */
    int DSlopeA;
    /** deck max beam width in millimeter */
    real DBW;
    /** deck aft width in millimeter */
    real DaftW;
    /** deck max beam width position in percent of deck length */
    int DBWPos;
    /** stem angle in degree from horizontal */
    int StemA;
    /** transom angle in degree from horizontal */
    int TransomA;

    	// bottom data
    /** bottom chine Length in millimeter */
    real BLWL;
    /** bottom forward height in millimeter */
    real BfwdHeight;
    /** bottom aft height in millimeter */
    real BaftHeight;
    /** bottom chine side slope angle in degree from horizontal */
    int BSlopeA;
    /** bottom chine max beam width in millimeter */
    real BBW;
    /** bottom chine aft width in millimeter */
    real BaftW;
    /** bottom chine max beam width position in percent of deck length */
    int BBWPos;
    /** bottom plank dead rise angle in degree from horizontal */
    int BDeadriseA;
    /** bottom sweep angle in degree forward of transverse axis */
    int BSweepA;
    /** Lower chine forward shape */
    int BfwdShape;
    //* Lower chine aft shape */
    int BaftShape;

        //planking data
    /** Number of side planks */
    int NBPlank;
    /** auto planking chech box */
    bool AutoPlank;
    /** top plank angle in degree from horizontal */
    int TopPlankA;
    /** lower plank angle in degree from horizontal */
    int LowPlankA;
};

#endif
