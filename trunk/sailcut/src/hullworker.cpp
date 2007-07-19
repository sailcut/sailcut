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


/** The constructor does some preliminary lower chine calculations 
 *  to set internal variables
 *  and compute lower chine plane.
 */
CHullWorker::CHullWorker(const CHullDef &d) : CHullDef(d)
{
    ptFwdChine = CPoint3d( 0 , BfwdHeight , 0 );
    ptAftChine = CPoint3d( BLWL , BaftHeight , BaftW/2 ); // transom end of lower chine 
    xBmax = real(DBWPos) * BLWL / 100; 
    
    CPoint3d p1, p2;
    /* print debug
        QString txt;
        txt = "point " + QString::number (j)+" ----   x = " + QString::number (p2.x()) + "  y = " + QString::number (p2.y()) + "  z = "+ QString::number (p2.z());
        qDebug ( txt.toLocal8Bit() );
    */
    // compute the vertical central plane
    CVector3d v1 = CVector3d( 1 , 0 , 0 );
    CVector3d v2 = CVector3d( 0 , 1 , 0 );
    planeCentral = CSubSpace3d::plane( ptFwdChine , v1 , v2 );
    
    /// compute the lower Chine plane
    // vector v1 is sideway tilt of chine
    v1 = CVector3d( 0 , -sin(real(BSlopeA)*PI/180) , cos(real(BSlopeA)*PI/180) );
    // vector v2 is fore-aft slope of deck
    v2 = CVector3d( ptAftChine - ptFwdChine );
    planeLowChine = CSubSpace3d::plane( ptFwdChine , v1 , v2 );
     
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
     
    /* laying lower chine panel from centre line to chine */ 
    unsigned int j;
    //unsigned int npl = deck.right.nbpoints();   // number of right/left points
    unsigned int npb = chine.bottom.nbpoints();   // number of bottom/top points
    
    chine.bottom.fill(ptFwdChine , ptCentreChine);   // centre line
    chine.top.fill(ptFwdChine , ptAftChine);         // edge
    deck.left.fill(ptFwdChine, ptFwdChine);          // stem
    deck.right.fill(ptCentreChine , ptAftChine);     // transom
    for ( j=0 ; j < npb ; j++)
    {   // move point to edge of deck
        p1 = chine.top.point[j];        
        p2 = ptLowChine( p1.x() );
        chine.top.point[j] = p2;
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
    QString txt;

    real x1 = 0 , y = 0 , z = 0;
    
    if (x > xBmax)
    {   // aft part of chine
        x1 = (x - xBmax) / (ptAftChine.x() - xBmax);
        z  = (.5 * BBW) + .5* (BaftW - BBW)* pow(x1 , BaftShape);
    }
    else
    {   // fwd part of chine
        x1 = 1 - ( x / xBmax );
        z  = (.5 * BBW)  *(1- pow(x1 , BfwdShape));
    }
    
    // point pt with x input and z computed
    CPoint3d pt = CPoint3d (x , y , z);
    // define vertical line1 passing through point pt
    CSubSpace line1;
    line1 =  CSubSpace3d::line (pt , CVector3d (0, 1, 0) );

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
    QString txt;

    // point pt with x input and z computed
    CPoint3d pt = ptLowChine ( x );
    // vector with deadrise and sweep
    CVector3d v1 = CVector3d( tan(real(-BSweepA) * PI/180) , tan(real(-BDeadriseA) * PI/180) , -1 );
;
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
CPanelGroup CHullWorker::makeHull() const
{
    CPanel deck1, deck2, side, side1, side2;
    unsigned int j;
    CPoint3d pt , pt0 , p1 , p2 , p3 , p4;
    CVector3d v1 , v2 , v3 , vg;
    
    /* array used to identify points for stem intersection 
     * the integer gives the  point on the top side of a side panel
     * which project on the stem bottom side point of the panel
     */
    unsigned int ptStem[NBPlank +1];
     
    CSubSpace Plane1;
    CSubSpace Line1;
    CSubSpace Intersection1;
    
    /* all the code has to be changed to start building the hull from the lower chine */
    unsigned int npl = deck.right.nbpoints();     // number of right/left points
    unsigned int npb = chine.bottom.nbpoints();   // number of bottom/top points
    
    /// Laying chine deck panels
    CPanelGroup hull(chine);
    hull.type = HULL;
    hull.title = hullID;
    deck1 = chine;
    
    for ( j=0 ; j < npb ; j++)
    {   // mirror points
        deck1.top.point[j].z() = -deck1.top.point[j].z();        
        deck1.bottom.point[j].z() = -deck1.bottom.point[j].z();        
    }
    for ( j=0 ; j < npl ; j++)
    {   // mirror points
        deck1.left.point[j].z() = -deck1.left.point[j].z();        
        deck1.right.point[j].z() = -deck1.right.point[j].z();        
    }
    hull.panel.push_back(deck1);

    /*// prepare for top side 
    pt = deck1.top.point[1];
    v1 = CVector3d( 1 , 0 , 0 );
    v2 = CVector3d( 0 , -sin(real(TopPlankA) * PI/180) , -cos(real(TopPlankA) * PI/180) );
    vg = v2;
    v3 = CVector3d( cos(real(StemA) * PI/180) , -sin(real(StemA) * PI/180) , 0 );
    
    Plane1 = CSubSpace3d::plane( pt , v1 , v2 );
    Line1 = CSubSpace3d::line( pt , v3 );
    
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
    
    /// search for the point of deck edge which projects at stem lower point
    p4 = pt;
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
    // keep memory of deck point number
    ptStem[NBPlank +1] = j;
    
    // compute previous point on chine plane 
    p1 = deck1.top.point[ptStem[NBPlank + 1] - 1];
    Line1 = CSubSpace3d::line( p1 , vg );
    Intersection1 = Line1.intersect(Plane1);
    if (Intersection1.getdim() == 0)
    {
        p3 = Intersection1.getp();
    }
    else throw "ERROR in CHullWorker::makeHull() Intersection 3 is not a point" ;
    
    //interpolate from previous point to find proper point on deck edge 
    a = p4.z() - p3.z();
    b = p4.z();
    // move deck point to fit 
    v1 = CVector3d( p1 - p2 )*(b / a);
    pt = p2 + v1;
    // move accordingly the deck point
    deck1.top.point[ptStem[NBPlank +1]] = pt;
    if (deck1.top.point[ptStem[NBPlank +1] +1].x() < pt.x() )
        deck1.top.point[ptStem[NBPlank +1] +1].x() = pt.x();
        
    /// generate side 1
    //  trim first part to stem line
    for ( j = 0 ; j <= ptStem[NBPlank +1] ; j++)
    {
        side1.top.point[j] = deck1.top.point[j];
        Line1 = CSubSpace3d::line( side1.top.point[j] , vg );
        Intersection1 = Line1.intersect(planeCentral);
        if (Intersection1.getdim() == 0)
        {
            pt = Intersection1.getp();
        }
        else throw "ERROR in CHullWorker::makeHull() Intersection 4 is not a point" ;
        
        side1.bottom.point[j] = pt;
    }
    
    for ( j = ptStem[NBPlank + 1] +1 ; j < side1.top.nbpoints() ; j++)
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
    side1.right.fill(side1.bottom.point[side1.bottom.nbpoints()-1], side1.top.point[side1.top.nbpoints()-1]);
    hull.panel.push_back(side1);
    
    /// create symetrical half deck ///
    for ( j = 0 ; j < deck1.top.nbpoints() ; j++ )
    {
        pt = deck1.top.point[j];  // deck edge
        pt.z() = -pt.z();
        deck2.top.point[j] = pt;
        
        pt = deck1.bottom.point[j];  //centre line
        pt.z() = -pt.z();
        deck2.bottom.point[j] = pt;
    } 

    for (j=0 ; j < deck1.left.nbpoints() ; j++ )
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
    for ( j = 0 ; j < side1.top.nbpoints() ; j++ )
    {
        pt = side1.top.point[j];
        pt.z() = -pt.z();
        side2.top.point[j] = pt;
        
        pt = side1.bottom.point[j];
        pt.z() = -pt.z();
        side2.bottom.point[j] = pt;
    } 

    for (j=0 ; j < side1.left.nbpoints() ; j++ )
    {
        pt = side1.left.point[j];
        pt.z() = -pt.z();
        side2.left.point[j] = pt;
        
        pt = side1.right.point[j];
        pt.z() = -pt.z();
        side2.right.point[j] = pt;
    } 
    hull.panel.push_back(side2);
    
    /// make transom first half
    unsigned int npb = deck1.right.nbpoints();
    unsigned int npl = side1.right.nbpoints();
    side.top = CSide(npb);
    side.bottom = CSide(npb);
    side.right = CSide(npl);
    side.left = CSide(npl);
    
    for (j = 0; j < npb; j++)
    {
        side.top.point[j] = deck1.right.point[j];
    }
    for (j = 0; j < npl; j++)
    {
        side.right.point[j] = side1.right.point[j];
    }
    side.bottom.point[npb-1] = side.right.point[0];
    side.bottom.point[0] = side.bottom.point[npb-1];
    side.bottom.point[0].z() = 0;
    side.bottom.fill(side.bottom.point[0], side.bottom.point[npb-1]);
    side.left.fill(side.bottom.point[0], side.top.point[0]);
    hull.panel.push_back(side);
    /// make transom second half
    for (j = 0; j < side.top.nbpoints(); j++)
    {
        side.top.point[j] = deck2.right.point[j];
    }
    for (j = 0; j < npl; j++)
    {
        side.right.point[j] = side2.right.point[j];
    }
    side.bottom.point[npb-1] = side2.right.point[0];
    side.bottom.point[0] = side.bottom.point[npb-1];
    side.bottom.point[0].z() = 0;
    side.bottom.fill(side.bottom.point[0], side.bottom.point[npb-1]);
    side.left.fill(side.bottom.point[0], side.top.point[0]);
    hull.panel.push_back(side);
    
    /// translate the hull such that stem is at x=O, y=0, z=0 ///
    for ( j=0; j < hull.panel.size(); j++ )
    {
        hull.panel[j] = hull.panel[j] + CVector3d(-pt0);
    }
    */
    return hull;
}


