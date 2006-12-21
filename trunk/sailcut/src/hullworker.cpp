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

#include "hullworker.h"


/** The constructor does some preliminary calculations to set
 *  internal variables.
 */
CHullWorker::CHullWorker(const CHullDef &d) : CHullDef(d)
{
}


/** Creates the deck.
 *
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CHullWorker::makeHull() const
{
    CPanel deck1, deck2, side;
    unsigned int j = 0;
    real d1 = 0;
    real mid = 1;

    CPoint3d p1(0, 0, 0);
    CPoint3d p2(LOA, 0, LOA/10);
    CPoint3d p3;
    CVector3d v1(1, 1, 1);

    unsigned int npl = deck1.right.nbpoints();   // number of right/left points
    unsigned int npb = deck1.bottom.nbpoints(); // number of bottom/top points

    ///* Start laying first half deck edge */
    deck1.top.fill(p1, p2);
    v1 = CVector3d(p2 - p1);
    mid = real(npb-1)/2;
    for ( j = 0 ; j < npb ; j++)
    {
        d1 = -(1 -(((real(j) - mid) / mid) * ((real(j) - mid) / mid))) * LOA / 8;
        deck1.top.point[j] = deck1.top.point[j] + CMatrix::rot3d(1, PI/2)*v1.unit()*d1;
    }
    /* make stem */
    for ( j = 0 ; j < npl-1 ; j++)
    {
        deck1.left.point[j] = p1;
    }
    /* make stern */
    p3 = deck1.top.point[npb-1];
    p3.z() = 0;
    deck1.right.fill(p3, deck1.top.point[npb-1]);
    /*
    v1 = CVector3d ( deck.right.point[npl-1] -deck.right.point[0]);
    for ( j = 0 ; j < npl-1 ; j++)
    {
        d1 = (1-((real(j) / (npl-1))*(real(j) / (npl-1))))* 0.1 * v1.norm();
        deck.right.point[j] = deck.right.point[j] + d1*CVector3d(1,0,0);
    }
    */
    /* make half deck lower edge on axis X */
    for ( j = 0 ; j < npb ; j++)
    {
        deck1.bottom.point[j] = deck1.top.point[j];
        deck1.bottom.point[j].z() = 0;
    }
    deck1.bottom.point[0] = deck1.left.point[0];
    deck1.bottom.point[npl-1] = deck1.right.point[0];

    ///* duplicate half deck and rotate panels around X axis to tilt sideways */
    real deck_angle = 0.1;
    deck2 = deck1.rotate(CPoint3d(0,0,0) , CMatrix::rot3d(0,PI - deck_angle) );
    deck1 = deck1.rotate(CPoint3d(0,0,0) , CMatrix::rot3d(0, deck_angle) );

    /* add first and second half of the deck to make hull */
    CPanelGroup hull(deck1);
    hull.type = HULL;
    hull.title = hullID;
    hull.panel.push_back(deck2);
    
    ///* make sides */
    v1 = CVector3d(LOA/50, -LOA/20, 0);
    for ( j = 0 ; j < npb ; j++)
    {
        side.top.point[j] = deck1.top.point[j];
        side.bottom.point[j] = side.top.point[j] + v1;
    }
    side.left.fill(side.bottom.point[0],side.top.point[0]);
    side.right.fill(side.bottom.point[npb-1], side.top.point[npb-1]);
    hull.panel.push_back(side);
    
    for ( j = 0 ; j < npb ; j++)
    {
        side.top.point[j] = deck2.top.point[j];
        side.bottom.point[j] = side.top.point[j] + v1;
    }
    side.left.fill(side.bottom.point[0],side.top.point[0]);
    side.right.fill(side.bottom.point[npb-1], side.top.point[npb-1]);
    hull.panel.push_back(side);
    
    return hull;
}


