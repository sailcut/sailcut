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
#include "sailcalc.h"


/** The constructor does some preliminary calculations to set
 *  internal variables.
 */
CHullWorker::CHullWorker(const CHullDef &d) : CHullDef(d)
{
    deckPt0 = CPoint3d( 0 , DfwdHeight , 0 );
    deckPt1 = CPoint3d( DLOA , DaftHeight , 0 ); // centre point of deck aft
    deckPt2 = deckPt1; 
    CPoint3d p1, p2;
    QString txt;
    /*
    txt = "point 0 ----   x = " + QString::number (p0.x() ) + "  y = " + QString::number (p0.y() ) + "  z = "+ QString::number (p0.z() );
    qDebug ( txt.toLocal8Bit() );
    */
    // compute the vertical central plane
    CVector3d v1 = CVector3d( 1 , 0 , 0 );
    CVector3d v2 = CVector3d( 0 , 1 , 0 );
    Pcentral = CSubSpace3d::plane( deckPt0 , v1 , v2 );
    
    /// compute the deck plane
    // vector v1 is sideway tilt of deck
    v1 = CVector3d( 0 , -sin(real(DSlopeA)*PI/180) , cos(real(DSlopeA)*PI/180) );
    // vector v2 is fore-aft slope of deck
    v2 = CVector3d(CPoint3d( deckPt1 - deckPt0 ) );
    Pdeck = CSubSpace3d::plane( deckPt0 , v1 , v2 );
     
    /// compute the transom plane
    // vector v1 parrallel to Z axis
    v1 = CVector3d( 0 , 0 , 1 ); 
    // vector v2 in inclined transom plane
    v2 = CVector3d( cos(real(TransomA) * PI/180) , sin(real(TransomA) * PI/180) , 0 );
    Ptransom = CSubSpace3d::plane( deckPt1 , v1 , v2 );
    
    // compute intersection line between deck and transom
    CSubSpace Line1;
    Line1 = Pdeck.intersect(Ptransom);
    
    // compute intersection point of line1 with central plane located at aft width
    CSubSpace Intersection2;
    if (Line1.getdim() == 1)
    {
        v1 = CVector3d( 1 , 0 , 0 );
        v2 = CVector3d( 0 , 1 , 0 );
        p2 = deckPt1 + CVector3d(0 , 0, DaftW/2); // initial deck aft edge point
        CSubSpace Plane1 = CSubSpace3d::plane( p2 , v1 , v2 );
        // compute intersection point at real aft edge of deck
        Intersection2 = Line1.intersect(Plane1);
        if (Intersection2.getdim() == 0)
            deckPt2 = Intersection2.getp();
        else throw "ERROR in hullworker constructor = no deck aft edge point";
    }
    else throw "ERROR in hullworker constructor = intersection deck transom is not a line";
     
    /* laying deck edge */ 
    unsigned int j;
    //unsigned int npl = deck.right.nbpoints();   // number of right/left points
    unsigned int npb = deck.bottom.nbpoints(); // number of bottom/top points
    
    deck.bottom.fill(deckPt0 , deckPt1);   // centre line
    deck.top.fill(deckPt0 , deckPt2);      // deck edge
    deck.left.fill(deckPt0 , deckPt0);     // stem
    deck.right.fill(deckPt2 , deckPt1);    // transom
    for ( j=0 ; j <= npb-1 ; j++)
    {   // move point to edge of deck
        p1 = deck.top.point[j];        
        deck.top.point[j] = DeckPt( p1.x() );
    }
}

/** Return the 3D point at the deck edge function of x
 *  x is the absisse of the point along the centre line
 *  The deck edge curve is a power curve on either side of the maximum beam point
 *
 * @author Robert Laine
 */
CPoint3d CHullWorker::DeckPt( const real &x )
{
    /*
      X (0..1) is the relative position of the point along the edge
      P is the integer position of the maximum round in percent of the edge length
      The curve is a parabola on either side of the point P 
      Return Y (0..1) is the value of the normalised round at position X
    */
    QString txt;

    real x1 = 0 , y = 0, z = 0;
    // compute position of max beam
    real pBmax = real(DBWPos) * DLOA / 100; 
    
    if (x > pBmax)
    {   // aft part of deck
        x1 = (x - pBmax) / (deckPt2.x() - pBmax);
        z  = (.5 * DBW) + .5* (DaftW - DBW)* pow(x1 , DaftShape);
        /* 
        txt = "pt        x1 = " + QString::number (x1) + "  z = "+ QString::number (z);
        qDebug ( txt.toLocal8Bit() );
        */
    }
    else
    {   // fwd part of deck
        x1 = 1 - ( x / pBmax );
        z  = (.5 * DBW)  - pow(x1 , DfwdShape);
    }
    
    // point pt with x input and z computed
    CPoint3d pt = CPoint3d (x , y , z);
    // define vertical line1 passing through point pt
    CSubSpace line1;
    line1 =  CSubSpace3d::line (pt , CVector3d (0, 1, 0) );

    // project pt vertically on real deck
    CSubSpace Intersection1;
    Intersection1 = Pdeck.intersect(line1);
    
    if (Intersection1.getdim() == 0 )
        pt = Intersection1.getp();
    else
        throw "ERROR in hullworker computing deck point is not a point";
    
    return pt;
}


/** Creates the deck.
 *
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CHullWorker::makeHull() const
{
    CPanel deck1, deck2, side1, side2, side;
    
    CPoint3d p0 = deckPt0;
    CPoint3d p1 = deckPt1;
    CPoint3d p2 = deckPt2;
    CPoint3d p3;
    CVector3d v1(1, 1, 1);

    /// Start laying first half deck edge
    deck1 = deck;
    

    /*// duplicate half deck and rotate panels around X axis to tilt sideways
    real deck_angle = real(DSlopeA) * PI/180;
    deck2 = deck1.rotate(CPoint3d(0,0,0) , CMatrix::rot3d(0,PI + 2* deck_angle) );
    */

    /* add first and second half of the deck to make hull 
    CPanelGroup hull(deck1);
    hull.type = HULL;
    hull.title = hullID;
    hull.panel.push_back(deck2);
    */
    
    /*
    // make sides 
    v1 = CVector3d(LOA/50, -LOA/20, 0);
    for ( j = 0 ; j < npb ; j++)
    {
        side1.top.point[j] = deck1.top.point[j];
        side1.bottom.point[j] = side1.top.point[j] + v1;
    }
    side1.left.fill(side1.bottom.point[0],side1.top.point[0]);
    side1.right.fill(side1.bottom.point[npb-1], side1.top.point[npb-1]);
    hull.panel.push_back(side1);
    
    for ( j = 0 ; j < npb ; j++)
    {
        side2.top.point[j] = deck2.top.point[j];
        side2.bottom.point[j] = side2.top.point[j] + v1;
    }
    side2.left.fill(side2.bottom.point[0],side2.top.point[0]);
    side2.right.fill(side2.bottom.point[npb-1], side2.top.point[npb-1]);
    hull.panel.push_back(side2);
    
    /// make stern
    side.top = CSide (npl);
    side.bottom = CSide(npl);
    for (j = 0; j < npl; j++)
    {
        side.top.point[j] = deck1.right.point[j];
    }
    side.bottom.point[0] = side1.bottom.point[npb-1];
    side.bottom.point[0].z() = 0;
    side.bottom.point[npl-1] = side1.bottom.point[npb-1];
    side.bottom.fill(side.bottom.point[0], side.bottom.point[npl-1]);
    side.left.fill(side.bottom.point[0], side.top.point[0]);
    side.right.fill(side.bottom.point[npl-1], side.top.point[npl-1]);
    hull.panel.push_back(side);
    
    for (j = 0; j < npl; j++)
    {
        side.top.point[j] = deck2.right.point[j];
    }
    //side.bottom.point[0] = side.top.point[0] + v1;
    side.bottom.point[npl-1] = side2.bottom.point[npb-1];
    side.bottom.fill(side.bottom.point[0], side.bottom.point[npl-1]);
    side.left.fill(side.bottom.point[0], side.top.point[0]);
    side.right.fill(side.bottom.point[npl-1], side.top.point[npl-1]);
    hull.panel.push_back(side);
    */ 
    return hull;
}


