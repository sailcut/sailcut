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

#include <cmath>

#include "hullworker.h"


/** The constructor does some preliminary calculations
 *  to set internal variables, compute chine and deck plane,
 *  and compute lower chine plane panel.
 *
 * @author Robert Laine
*/
CHullWorker::CHullWorker(const CHullDef &d) : CHullDef(d)
{
    ptFwdChine = CPoint3d( 0 , BfwdHeight , 0 );
    ptAftChine = CPoint3d( BLWL , BaftHeight , BaftW/2 ); // transom end of lower chine
    xBmax = real(DBWPos) * BLWL / 100;
    ptFwdDeck = CPoint3d( 0 , DfwdHeight , 0 );
    CPoint3d p1, p2;

    /** compute the vertical central plane */
    CVector3d v1 = CVector3d( 1 , 0 , 0 );
    CVector3d v2 = CVector3d( 0 , 1 , 0 );
    planeCentral = CSubSpace::plane( ptFwdChine , v1 , v2 );

    /** compute the lower Chine plane */
    // vector v1 is sideway tilt of chine
    v1 = CVector3d( 0 , -sin(degreesToRadians(BSlopeA)) , cos(degreesToRadians(BSlopeA)) );
    // vector v2 is fore-aft slope of chine
    v2 = CVector3d( ptAftChine - ptFwdChine );
    planeLowChine = CSubSpace::plane( ptFwdChine , v1 , v2 );

    /** compute the Deck plane */
    // vector v1 is sideway tilt of deck
    v1 = CVector3d( 0 , -sin(degreesToRadians(DSlopeA)) , cos(degreesToRadians(DSlopeA)) );
    // vector v2 is fore-aft slope of deck
    v2 = CVector3d( BLWL , DaftHeight-DfwdHeight , 0 );
    planeDeck = CSubSpace::plane( ptFwdDeck , v1 , v2 );

    /** compute the transom plane */
    // vector v1 is parallel to Z axis = perpendicular to central plane
    v1 = CVector3d( 0 , 0 , 1 );
    // vector v2 is in inclined transom plane
    v2 = CVector3d( cos(degreesToRadians(TransomA)) , sin(degreesToRadians(TransomA)) , 0 );
    planeTransom = CSubSpace::plane( ptAftChine , v1 , v2 );

    // compute intersection line between lower chine plane and transom
    CSubSpace Line1 = planeLowChine.intersect(planeTransom);
    if (Line1.getdim() != 1)
        throw "ERROR in hullworker constructor = intersection chine plane and transom is not a line";

    // compute intersection point of line1 with central plane located at aft width
    ptCentreChine = Line1.intersectionPoint(planeCentral, "low chine aft");

    /** lay lower chine plane decking panel */
    unsigned int j;
    //unsigned int npl = deck.right.size();   // number of right/left points
    unsigned int npb = chine.bottom.size();   // number of bottom/top points

    chine.top.fill( ptFwdChine , ptCentreChine );      // top edge at centre line
    chine.bottom.fill( ptFwdChine , ptAftChine );      // bottom edge at chine
    chine.left.fill( ptFwdChine , ptFwdChine );        // stem is left edge
    chine.right.fill( ptAftChine , ptCentreChine );    // transom is right edge
    for (j=0 ; j < npb ; j++)
    {   // move bottom point to chine
        p1 = chine.bottom[j];
        p2 = ptLowChine( p1.x() );
        chine.bottom[j] = p2;
    }
}


/** Return the 3D point at the Lower chine edge function of x
 *  x is the absisse of the point along the centre line
 *  The chine edge curve is a power curve on either side of the maximum beam point
 *
 * @author Robert Laine
 */
CPoint3d CHullWorker::ptLowChine( const real &x )
{
    real x1 = 0 , y = 0 , z = 0;

    /* compute width z function of x */
    if (x > xBmax)
    {   // aft part of chine
        x1 = (x - xBmax) / (ptAftChine.x() - xBmax);
        z  = (.5 * BBW) + .5* (BaftW - BBW)* pow(x1 , BaftShape);
    }
    else
    {   // fwd part of chine
        x1 = 1 - ( x / xBmax );
        z  = (.5 * BBW) * (1- pow(x1 , BfwdShape) );
    }

    /* create point with x input, y=0 and z computed */
    CPoint3d pt = CPoint3d ( x , y , z );

    /* compute y by projecting pt vertically on the chine plane */
    return planeLowChine.intersectionPoint(CSubSpace::line(pt, CVector3d(0, 1, 0)), "low chine");
}


/** Return the 3D point at the keel line function of x
 *  x is the absisse of the corresponding point on the lower chine
 *
 * @author Robert Laine
 */
CPoint3d CHullWorker::ptKeel( const real &x )
{
    CPoint3d pt;
    CVector3d v1;
    // create point with x input and y, z computed
    pt = ptLowChine ( x );

    /* Ruling vector with deadrise and sweep. */
    v1 = CVector3d( tan(degreesToRadians(BSweepA)) , tan(degreesToRadians(BDeadriseA)) , 1 );

    /* Project pt on central plane, get intersection
       with the ruling line passing through point pt. */
    return planeCentral.intersectionPoint(CSubSpace::line(pt, v1), "keel");
}


/** Creates the basic hull with bottom, deck
 *  and only one top side plank.
 *
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CHullWorker::makeHull() //const
{
    CPanel deck1, deck2, plank1, plank2;
    unsigned int j;
    real x;
    CPoint3d pt , pt0 , p1 , p2 , p3 , p4;
    CVector3d v1 , v2 , v3 , vg;

    // create the hull type
    CPanelGroup hull;
    hull.type = HULL; // used for color scheme in saildispgl
    hull.title = hullID;

    /** NOTE: the code will have to be changed to build the full hull from the lower chine */
    unsigned int npl = chine.right.size();     // number of right/left points
    unsigned int npb = chine.bottom.size();   // number of bottom/top points

    deck1 = chine;
    plank1 = chine;

    /** Lay the two bottom panels with top edge at centre plane */
    for (j = 0 ; j < npb ; j++)
    {
        plank1.bottom[j] =  chine.bottom[j];
        x = chine.bottom[j].x();
        plank1.top[j] = ptKeel( x );
    }
    plank1.left.fill( plank1.bottom[0] , plank1.top[0] );
    // NOTE: bottom right edge incomplete until code is added to cope with sweep
    plank1.right.fill( plank1.bottom[npb-1] , plank1.top[npb-1] );

    plank2 = plank1;
    for (j = 0 ; j < npb ; j++)
    {   // mirror points
        plank2.top[j].setZ(-plank1.top[j].z());
        plank2.bottom[j].setZ(-plank1.bottom[j].z());
    }
    for (j = 0 ; j < npl ; j++)
    {   // mirror points
        plank2.left[j].setZ(-plank1.left[j].z());
        plank2.right[j].setZ(-plank1.right[j].z());
    }
    // add the bottom planks to the hull
    hull.push_back(plank1);
    hull.push_back(plank2);

    /** Lay a single top side plank on each side with bottom at chine */
    v1 = CVector3d( -cos(degreesToRadians(StemA)) , sin(degreesToRadians(TopPlankA)) , cos(degreesToRadians(TopPlankA)) );

    for (j = 0 ; j < npb ; j++)
    {
        plank1.bottom[j] =  chine.bottom[j];
        p1 = planeDeck.intersectionPoint(CSubSpace::line(plank1.bottom[j], v1), "top side");
        // compute the vector vg which generate the side surface
        vg = CVector3d(p1 - pt);
        plank1.top[j] = p1;
    }
    plank1.left.fill( plank1.bottom[0] , plank1.top[0] );
    plank1.right.fill( plank1.bottom[npb-1] , plank1.top[npb-1] );

    plank2 = plank1;
    for (j = 0 ; j < npb ; j++)
    {   // mirror points
        plank2.top[j].setZ(-plank1.top[j].z());
        plank2.bottom[j].setZ(-plank1.bottom[j].z());
    }
    for (j = 0 ; j < npl ; j++)
    {   // mirror points
        plank2.left[j].setZ(-plank1.left[j].z());
        plank2.right[j].setZ(-plank1.right[j].z());
    }
    // add the top side planks to the hull
    hull.push_back(plank1);
    hull.push_back(plank2);

    /** Lay the two deck planks */
    for (j = 0 ; j < npb ; j++)
    {
        deck1.bottom[j] = plank1.top[j];
        deck1.top[j] = deck1.bottom[j];
        deck1.top[j].setZ(0);
        deck1.left.fill(deck1.bottom[0] , deck1.top[0]);
        deck1.right.fill(deck1.bottom[npb-1] , deck1.top[npb-1]);

        deck2.bottom[j] = plank2.top[j];
        deck2.top[j] = deck2.bottom[j];
        deck2.top[j].setZ(0);
        deck2.left.fill(deck2.bottom[0] , deck2.top[0]);
        deck2.right.fill(deck2.bottom[npb-1] , deck2.top[npb-1]);
    }
    // add the deck planks to the hull
    hull.push_back(deck1);
    hull.push_back(deck2);

    // translate the hull such that stem is at x=O, y=0, z=0 ///
    j =  hull.size() -1;
    pt0 = hull[j].top[0];

    for (j = 0 ; j < hull.size() ; j++)
    {
        hull[j] = hull[j] + CVector3d(-pt0);
    }
    return hull;
}


