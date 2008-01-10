/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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

#ifndef HULLWORKER_H
#define HULLWORKER_H

#include "panelgroup.h"
#include "hulldef.h"


/** The CHullWorker class does all the hull-related calculations.
 *  It is used to create the hull from its definition.
 *
 * @see CHullDef, CPanelGroup
 */
class CHullWorker : public CHullDef
{
public:
    CHullWorker(const CHullDef &d);

    CPanelGroup makeHull();

protected:
    /** abciss of Bmax point on chine */
    real xBmax;
    
    /** function compute a point on Deck edge  */
    CPoint3d ptDeck( const real &x );
    /** function compute a point on Keel line */
    CPoint3d ptKeel( const real &x);
    /** function compute a point on chine n */
    CPoint3d ptChine(const int &n, const real &x );
    /** function compute a point on lower Chine */
    CPoint3d ptLowChine( const real &x );
    
    /** central plane longitudinal X-Y */
    CSubSpace planeCentral;
    /** deck plane */
    CSubSpace planeDeck;
    /** transom plane */
    CSubSpace planeTransom;
    /** chine plane */
    CSubSpace planeChine[5];
    CSubSpace planeLowChine;
    
    /** Lower Chine point at stem/centre line */
    CPoint3d ptFwdChine;
    /** Lower Chine point at transom/edge */
    CPoint3d ptAftChine;
    /** Lower Chine point at transom/centre line */
    CPoint3d ptCentreChine;
    
    /** Deck Forward Point */
    CPoint3d ptFwdDeck;
    
    /** basic panels */
    CPanel deck;
    CPanel plank;
    CPanel chine;
    
};

#endif
