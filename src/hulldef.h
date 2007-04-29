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

#ifndef HULLDEF_H
#define HULLDEF_H

#include <QString>
#include <geocpp/geocpp.h>


/** This class holds the parameters that define a hull
 *  and the default parameters.
 *
 * @see CHullWorker
 */
class CHullDef
{
public:
    CHullDef();

    CHullDef & operator=( const CHullDef & );

    /** hull ID name */
    QString hullID;
    /** deck Length in millimeter */
    real DLOA;
    /** deck forward height in millimeter */
    real DfwdHeight;
    /** deck aft height in millimeter */
    real DaftHeight;
    /** deck side slope angle in degree from horizontal */
    unsigned int DSlopeA;
    /** deck max beam width in millimeter */
    real DBW;
    /** deck aft width in millimeter */
    real DaftW;
    /** deck max beam width position in percent of deck length */
    unsigned int DBWPos;
    /** stem angle in degree from horizontal */
    unsigned int StemA;
    /** transom angle in degree from horizontal */
    unsigned int TransomA;
    /** deck forward shape coefficient (power of x) */
    unsigned int DfwdShape;
    /** deck aft shape coefficient (power of x) */
    unsigned int DaftShape;
    	// bottom data
    /** bottom chine Length in millimeter */
    real BLOA;
    /** bottom forward height in millimeter */
    real BfwdHeight;
    /** bottom aft height in millimeter */
    real BaftHeight;
    /** bottom chine side slope angle in degree from horizontal */
    unsigned int BSlopeA;
    /** bottom chine max beam width in millimeter */
    real BBW;
    /** bottom chine aft width in millimeter */
    real BaftW;
    /** bottom chine max beam width position in percent of deck length */
    unsigned int BBWPos;
    /** bottom plank dead rise angle in degree from horizontal */
    unsigned int BdeadriseA;
    /** bottom sweep angle in degree forward of transverse axis */
    unsigned int BsweepA;
    //planking data
    /** Number of side planks */
    unsigned int NBPlank;
    /** auto planking chech box */
    bool AutoPlank;
    /** top plank angle in degree from horizontal */
    unsigned int TopPlankA;
    /** lower plank angle in degree from horizontal */
    unsigned int LowPlankA;
    
};

#endif
