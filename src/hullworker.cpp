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
    deckPt0 = CPoint3d( 0 , DfwdHeight , 0 );
    deckPt1 = CPoint3d( DLOA , DaftHeight , 0 ); // centre point of deck aft
    deckPt2 = deckPt1; 
    CPoint3d p1, p2;
    /* print debug
        QString txt;
        txt = "point " + QString::number (j)+" ----   x = " + QString::number (p2.x()) + "  y = " + QString::number (p2.y()) + "  z = "+ QString::number (p2.z());
        qDebug ( txt.toLocal8Bit() );
    */
    // compute the vertical central plane
    CVector3d v1 = CVector3d( 1 , 0 , 0 );
    CVector3d v2 = CVector3d( 0 , 1 , 0 );
    centralPlane = CSubSpace3d::plane( deckPt0 , v1 , v2 );
    
    /// compute the deck plane
    // vector v1 is sideway tilt of deck
    v1 = CVector3d( 0 , -sin(real(DSlopeA)*PI/180) , cos(real(DSlopeA)*PI/180) );
    // vector v2 is fore-aft slope of deck
    v2 = CVector3d( deckPt1 - deckPt0 );
    deckPlane = CSubSpace3d::plane( deckPt0 , v1 , v2 );
     
    /// compute the transom plane
    // vector v1 is parrallel to Z axis = perpendicular to central plane
    v1 = CVector3d( 0 , 0 , 1 ); 
    // vector v2 is in inclined transom plane
    v2 = CVector3d( cos(real(TransomA) * PI/180) , sin(real(TransomA) * PI/180) , 0 );
    transomPlane = CSubSpace3d::plane( deckPt1 , v1 , v2 );
    
    // compute intersection line between deck and transom
    CSubSpace Line1;
    Line1 = deckPlane.intersect(transomPlane);
    
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
        p2 = DeckPt( p1.x() );
        deck.top.point[j] = p2;
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
    QString txt;

    real x1 = 0 , y = 0, z = 0;
    // compute position of max beam
    real pBmax = real(DBWPos) * DLOA / 100; 
    
    if (x > pBmax)
    {   // aft part of deck
        x1 = (x - pBmax) / (deckPt2.x() - pBmax);
        z  = (.5 * DBW) + .5* (DaftW - DBW)* pow(x1 , DaftShape);
    }
    else
    {   // fwd part of deck
        x1 = 1 - ( x / pBmax );
        z  = (.5 * DBW)  *(1- pow(x1 , DfwdShape));
    }
    
    // point pt with x input and z computed
    CPoint3d pt = CPoint3d (x , y , z);
    // define vertical line1 passing through point pt
    CSubSpace line1;
    line1 =  CSubSpace3d::line (pt , CVector3d (0, 1, 0) );

    // project pt vertically on real deck
    CSubSpace Intersection1;
    Intersection1 = deckPlane.intersect(line1);
    
    if (Intersection1.getdim() == 0 )
        pt = Intersection1.getp();
    else
        throw "ERROR in CHullWorker::DeckPt intersection 0 point is not a point";
    
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
    unsigned int j , j1;
    real a , b;
    CPoint3d pt , p1 , p2 , p3 , p4;
    CVector3d v1 , v2 , v3 , vg;
    
    CSubSpace Plane1;
    CSubSpace Line1;
    CSubSpace Intersection1;

    /// Start laying first half deck edge
    deck1 = deck;
    CPanelGroup hull(deck1);
    hull.type = HULL;
    hull.title = hullID;
    
    unsigned int npl = deck.right.nbpoints();   // number of right/left points
    unsigned int npb = deck.bottom.nbpoints(); // number of bottom/top points
    
    /// prepare for top side 
    pt = deck1.top.point[1];
    v1 = CVector3d( 1 , 0 , 0 );
    v2 = CVector3d( 0 , -sin(real(TopPlankA) * PI/180) , -cos(real(TopPlankA) * PI/180) );
    vg = v2;
    v3 = CVector3d( cos(real(StemA) * PI/180) , -sin(real(StemA) * PI/180) , 0 );
    
    Plane1 = CSubSpace3d::plane( pt , v1 , v2 );
    Line1 = CSubSpace3d::line( deckPt0 , v3 );
    
        // compute intersection of stem and side inclination plane
    Intersection1 = Line1.intersect(Plane1);
    if (Intersection1.getdim() == 0)
    {
        // compute the vector vg which generate the side surface
        p1 = Intersection1.getp();
        vg = CVector3d(p1 - pt);
    }
    else throw "ERROR in CHullWorker::makeHull() Intersection 1 is not a point" ;
    
    /// define lower chine plane = Plane1
    v1 = CVector3d ( CPoint3d(DLOA, BaftHeight, 0) - CPoint3d(0, BfwdHeight, 0) );
    v2 = CVector3d ( 0 , -sin(real(BSlopeA)*PI/180) , cos (real(BSlopeA)*PI/180) );
    Plane1 = CSubSpace3d::plane( CPoint3d(0, BfwdHeight, 0) , v1 , v2);
    
    /// search for point of deck which project at stem lower point
    p4 = deckPt0;
    p4.z() = -1;
    j = 0;
    while ( p4.z() < 0 )
    {
        j++ ;

        p2 = deck1.top.point[j];
        Line1 = CSubSpace3d::line( p2 , vg );
        Intersection1 = Line1.intersect(Plane1);
        if (Intersection1.getdim() == 0)
        {
            p4 = Intersection1.getp();
        }
        else throw "ERROR in CHullWorker::makeHull() Intersection 2 is not a point" ;
    }
    
    j1 = j;
    /*    QString txt;
        txt = "pt   j1 = " + QString::number (j1) + "  ;  x = " + QString::number (p2.x());
        qDebug ( txt.toLocal8Bit() );
    */
    
    // compute previous point on chine plane 
    p1 = deck1.top.point[j-1];
    Line1 = CSubSpace3d::line( p1 , vg );
    Intersection1 = Line1.intersect(Plane1);
    if (Intersection1.getdim() == 0)
    {
        p3 = Intersection1.getp();
    }
    else throw "ERROR in CHullWorker::makeHull() Intersection 3 is not a point" ;
    
    //interpolate from last point to find proper point on deck edge 
    a = p4.z() - p3.z();
    b = p4.z();
    // move deck point to fit 
    v1 = CVector3d( p1 - p2 )*(b / a);
    pt = p2 + v1;
    // move accordingly the deck point
    deck1.top.point[j] = pt;
    // keep memory of deck point number
    j1 = j;
    
    /// generate side 1
    //  trim first part to stem line
    for ( j = 0 ; j <= j1 ; j++)
    {
        side1.top.point[j] = deck1.top.point[j];
        Line1 = CSubSpace3d::line( side1.top.point[j] , vg );
        Intersection1 = Line1.intersect(centralPlane);
        if (Intersection1.getdim() == 0)
        {
            pt = Intersection1.getp();
        }
        else throw "ERROR in CHullWorker::makeHull() Intersection 4 is not a point" ;
        
        side1.bottom.point[j] = pt;
    }
    for ( j = j1+1 ; j < npb ; j++)
    {
        side1.top.point[j] = deck1.top.point[j];
        Line1 = CSubSpace3d::line( side1.top.point[j] , vg );
        Intersection1 = Line1.intersect(Plane1);
        if (Intersection1.getdim() == 0)
        {
            pt = Intersection1.getp();
        }
        else throw "ERROR in CHullWorker::makeHull() Intersection 5 is not a point" ;
        
        side1.bottom.point[j] = pt;
    }
    
    side1.left.fill(side1.bottom.point[0],side1.top.point[0]);
    side1.right.fill(side1.bottom.point[npb-1], side1.top.point[npb-1]);
    hull.panel.push_back(side1);
    
    /// create symetrical half deck 
    for ( j = 0 ; j <= npb-1 ; j++ )
    {
        pt = deck1.top.point[j];
        pt.z() = -pt.z();
        deck2.top.point[j] = pt;
        
        pt = deck1.bottom.point[j];
        pt.z() = -pt.z();
        deck2.bottom.point[j] = pt;
    } 

    for (j=0 ; j <= npl-1 ; j++ )
    {
        pt = deck1.left.point[j];
        pt.z() = -pt.z();
        deck2.left.point[j] = pt;
        
        pt = deck1.right.point[j];
        pt.z() = -pt.z();
        deck2.right.point[j] = pt;
    } 
    hull.panel.push_back(deck2);
    
    /// make side 2 symetric by changing sign of z component of points
    for ( j = 0 ; j <= npb-1 ; j++ )
    {
        pt = side1.top.point[j];
        pt.z() = -pt.z();
        side2.top.point[j] = pt;
        
        pt = side1.bottom.point[j];
        pt.z() = -pt.z();
        side2.bottom.point[j] = pt;
    } 

    for (j=0 ; j <= npl-1 ; j++ )
    {
        pt = side1.left.point[j];
        pt.z() = -pt.z();
        side2.left.point[j] = pt;
        
        pt = side1.right.point[j];
        pt.z() = -pt.z();
        side2.right.point[j] = pt;
    } 
    hull.panel.push_back(side2);
    
    /// make stern
    /*
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


