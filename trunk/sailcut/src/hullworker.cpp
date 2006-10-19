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
CHullWorker::CHullWorker(const CHullDef &h) : CHullDef(h)
{
}


/** Creates the deck.
 *
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CHullWorker::makeHull() const
{
    CPanel deck;

    unsigned int j = 0;
    real d1 = 0;
    real mid = 1;

    CPoint3d p1(0, 0, LOA/100);
    CPoint3d p2(LOA, 0, LOA/10);
    CVector3d v1(1, 1, 1);

    unsigned int npl = deck.right.nbpoints();   // number of right/left points
    unsigned int npb = deck.bottom.nbpoints(); // number of bottom/top points

    /** Start laying deck edge */
    deck.top.fill(p1, p2);
    v1 = CVector3d(p2 - p1);
    mid = (npb-1)/2;
    for ( j = 1 ; j < npb-1 ; j++)
    {
        d1 = -(1- ((real(j) - mid) / mid) * ((real(j) - mid) / mid)) * LOA / 8;
        deck.top.point[j] = deck.top.point[j] + CMatrix::rot3d(1, PI/2)*v1.unit()*d1;
    }

    /** make panel lower edge symetrical of upper edge */
    for ( j = 0 ; j < npb ; j++)
    {
        deck.bottom.point[j] = deck.top.point[j];
        deck.bottom.point[j].z() = -deck.bottom.point[j].z();
    }

    /** make stem */
    deck.left.fill(deck.bottom.point[0], deck.top.point[0]);
    v1 = CVector3d ( deck.top.point[0] -deck.bottom.point[0]);
    mid = (npl -1) / 2;
    for ( j = 1 ; j < npl-1 ; j++)
    {
        d1 = -(1- ((real(j) - mid) / mid)* ((real(j) - mid) / mid)) * 0.3 * v1.norm();
        deck.left.point[j] = deck.left.point[j] + CMatrix::rot3d(1,PI/2)*v1.unit()*d1;
    }

    /** make stern */
    deck.right.fill(deck.bottom.point[npb-1], deck.top.point[npb-1]);
    v1 = CVector3d ( deck.top.point[npb-1] - deck.bottom.point[npb-1]);
    mid = (npl - 1) / 2;
    for ( j = 1 ; j < npl-1 ; j++)
    {
        d1 = -(1- ((real(j) - mid) / mid) * ((real(j) - mid) / mid)) * 0.1 * v1.norm();
        deck.right.point[j] = deck.right.point[j] + CMatrix::rot3d(1,-PI/2)*v1.unit()*d1;
    }

    CPanelGroup hull(deck);
    hull.type = HULL;
    return hull;
}


