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

#include <cmath>

#include "hullworker.h"


/** The constructor does some preliminary lower chine calculations
 *  to set internal variables
 *  and compute lower chine plane.
 */
CHullWorker::CHullWorker(const CHullDef &d) : CHullDef(d)
{
    ptFwdChine = CPoint3d( 0 , BfwdHeight , 0 );
    ptAftChine = CPoint3d( BLWL , BaftHeight , BaftW/2 ); // transom end of lower chine
    xBmax = real(DBWPos) * BLWL / 100;
    ptFwdDeck = CPoint3d( 0 , DfwdHeight , 0 );
    CPoint3d p1, p2;

    // compute the vertical central plane
    CVector3d v1 = CVector3d( 1 , 0 , 0 );
    CVector3d v2 = CVector3d( 0 , 1 , 0 );
    planeCentral = CSubSpace3d::plane( ptFwdChine , v1 , v2 );

    /// compute the lower Chine plane
    // vector v1 is sideway tilt of chine
    v1 = CVector3d( 0 , -sin(real(BSlopeA)*PI/180) , cos(real(BSlopeA)*PI/180) );
    // vector v2 is fore-aft slope of chine
    v2 = CVector3d( ptAftChine - ptFwdChine );
    planeLowChine = CSubSpace3d::plane( ptFwdChine , v1 , v2 );

    /// compute the Deck plane
    // vector v1 is sideway tilt of deck
    v1 = CVector3d( 0 , -sin(real(DSlopeA)*PI/180) , cos(real(DSlopeA)*PI/180) );
    // vector v2 is fore-aft slope of deck
    v2 = CVector3d( BLWL , DaftHeight-DfwdHeight , 0 );
    planeDeck = CSubSpace3d::plane( ptFwdDeck , v1 , v2 );

    /// compute the transom plane
    // vector v1 is parallel to Z axis = perpendicular to central plane
    v1 = CVector3d( 0 , 0 , 1 );
    // vector v2 is in inclined transom plane
    v2 = CVector3d( cos(real(TransomA) * PI/180) , sin(real(TransomA) * PI/180) , 0 );
    planeTransom = CSubSpace3d::plane( ptAftChine , v1 , v2 );

    // compute intersection line between chine plane and transom
    CSubSpace Line1;
    Line1 = planeLowChine.intersect(planeTransom);

    // compute intersection point of line1 with central plane located at aft width
    CSubSpace Intersection2;
    if (Line1.getdim() == 1)
    {
        Intersection2 = Line1.intersect(planeCentral);
        if (Intersection2.getdim() == 0)
            ptCentreChine = Intersection2.getp();
        else throw "ERROR in hullworker constructor = no low chine aft  point";
    }
    else throw "ERROR in hullworker constructor = intersection chine plane and transom is not a line";

    /* laying lower chine panel */
    unsigned int j;
    //unsigned int npl = deck.right.size();   // number of right/left points
    unsigned int npb = chine.bottom.size();   // number of bottom/top points

    chine.top.fill( ptFwdChine , ptCentreChine );      // centre line
    chine.bottom.fill( ptFwdChine , ptAftChine );      // edge
    chine.left.fill( ptFwdChine , ptFwdChine );        // stem
    chine.right.fill( ptAftChine , ptCentreChine );    // transom
    for (j=0 ; j < npb ; j++)
    {   // move point to edge of deck
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

    // point pt with x input and z computed
    CPoint3d pt = CPoint3d ( x , y , z );
    // define vertical Line1 passing through point pt
    CSubSpace line1;
    line1 =  CSubSpace3d::line( pt , CVector3d (0,1,0) );

    // project pt vertically on chine plane
    CSubSpace Intersection1;
    Intersection1 = planeLowChine.intersect(line1);

    if (Intersection1.getdim() == 0 )
        pt = Intersection1.getp();
    else
        throw "ERROR in CHullWorker::ptLowChine intersection is not a point";

    return pt;
}


/** Return the 3D point at the keel function of x
 *  x is the absisse of the corresponding point on the lower chine
 *
 * @author Robert Laine
 */
CPoint3d CHullWorker::ptKeel( const real &x )
{
    CPoint3d pt;
    CVector3d v1;
    // point pt with x input and z computed
    pt = ptLowChine ( x );

    // vector with deadrise and sweep
    v1 = CVector3d( tan( real(BSweepA) * PI/180) , tan( real(BDeadriseA) * PI/180) , 1 );

    // define ruling line 1 passing through point pt
    CSubSpace line1;
    line1 =  CSubSpace3d::line (pt , v1 );

    // project pt on central plane
    CSubSpace Intersection1;
    Intersection1 = planeCentral.intersect(line1);

    if (Intersection1.getdim() == 0 )
        pt = Intersection1.getp();
    else
        throw "ERROR in CHullWorker::ptKeel intersection is not a point";

    return pt;
}


/** Creates the hull.
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

    CSubSpace Plane1;
    CSubSpace Line1;
    CSubSpace Intersection1;

    // create the hull type
    CPanelGroup hull;
    hull.type = HULL; // used for color scheme in saildispgl
    hull.title = hullID;

    /* all the code has to be changed to start building the hull from the lower chine */
    unsigned int npl = chine.right.size();     // number of right/left points
    unsigned int npb = chine.bottom.size();   // number of bottom/top points

    deck1 = chine;
    plank1 = chine;

    // Laying bottom panels
    for (j = 0 ; j < npb ; j++)
    {
        plank1.bottom[j] =  chine.bottom[j];
        x = chine.bottom[j].x();
        plank1.top[j] = ptKeel( x );
    }
    plank1.left.fill( plank1.bottom[0] , plank1.top[0] );
    plank1.right.fill( plank1.bottom[npb-1] , plank1.top[npb-1] );

    plank2 = plank1;
    for (j = 0 ; j < npb ; j++)
    {   // mirror points
        plank2.top[j].z() = -plank1.top[j].z();
        plank2.bottom[j].z() = -plank1.bottom[j].z();
    }
    for (j = 0 ; j < npl ; j++)
    {   // mirror points
        plank2.left[j].z() = -plank1.left[j].z();
        plank2.right[j].z() = -plank1.right[j].z();
    }
    // add the bottom planks to the hull
    hull.push_back(plank1);
    hull.push_back(plank2);

    // laying top side planks
    v1 = CVector3d( -cos(real(StemA) * PI/180) , sin(real(TopPlankA) * PI/180) , cos(real(TopPlankA) * PI/180) );

    for (j = 0 ; j < npb ; j++)
    {
        plank1.bottom[j] =  chine.bottom[j];
        Line1 = CSubSpace3d::line( plank1.bottom[j] , v1 );
        Intersection1 = Line1.intersect(planeDeck);
        if (Intersection1.getdim() == 0)
        {   // compute the vector vg which generate the side surface
            p1 = Intersection1.getp();
            vg = CVector3d(p1 - pt);
        }
        else throw "ERROR in CHullWorker::makeHull() Intersection 1 top side is not a point" ;
        plank1.top[j] = p1;
    }
    plank1.left.fill( plank1.bottom[0] , plank1.top[0] );
    plank1.right.fill( plank1.bottom[npb-1] , plank1.top[npb-1] );

    plank2 = plank1;
    for (j = 0 ; j < npb ; j++)
    {   // mirror points
        plank2.top[j].z() = -plank1.top[j].z();
        plank2.bottom[j].z() = -plank1.bottom[j].z();
    }
    for (j = 0 ; j < npl ; j++)
    {   // mirror points
        plank2.left[j].z() = -plank1.left[j].z();
        plank2.right[j].z() = -plank1.right[j].z();
    }
    // add the top side planks to the hull
    hull.push_back(plank1);
    hull.push_back(plank2);

    // Laying deck planks
    for (j = 0 ; j < npb ; j++)
    {
        deck1.bottom[j] = plank1.top[j];
        deck1.top[j] = deck1.bottom[j];
        deck1.top[j].z() = 0;
        deck1.left.fill(deck1.bottom[0] , deck1.top[0]);
        deck1.right.fill(deck1.bottom[npb-1] , deck1.top[npb-1]);

        deck2.bottom[j] = plank2.top[j];
        deck2.top[j] = deck2.bottom[j];
        deck2.top[j].z() = 0;
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


