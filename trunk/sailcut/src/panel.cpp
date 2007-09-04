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

#include "panel.h"
#include "sailcalc.h"

/*****************************************************************************
 
                              CPanelLabel class
 
*****************************************************************************/

/** Default constructor */
CPanelLabel::CPanelLabel()
        : name("new panel"), height(5), color(1), origin(CPoint3d(0,0,0)), direction(CVector3d(0,0,0))
{}


/** Copy constructor.
 */
CPanelLabel::CPanelLabel( const CPanelLabel &lb )
        : name(lb.name), height(lb.height), color(lb.color), origin(lb.origin), direction(lb.direction)
{}


/** Rotates a label around a point.
 */
CPanelLabel CPanelLabel::rotate( const CPoint3d &p , const CMatrix &m ) const
{
    CPanelLabel lb;

    lb.name = name;
    lb.height = height;
    lb.color = color;
    lb.origin = p + m*(origin-p);
    lb.direction = m*direction;

    return lb;
}

// operators

/** Performs a 3D translation of a label by a given vector.
 */
CPanelLabel CPanelLabel::operator+ ( const CVector3d &transl ) const
{
    CPanelLabel lb;

    lb.name = name;
    lb.height = height;
    lb.color = color;
    lb.origin = origin + transl;
    lb.direction = direction;

    return lb;
}


/** Performs an assignment.
 */
CPanelLabel& CPanelLabel::operator= (const CPanelLabel &lb)
{
    if ( &lb == this )
    {
        CException e("CPanelLabel::operator= : self-assignment!");
        throw(e);
    }

    name = lb.name;
    height = lb.height;
    color = lb.color;
    origin = lb.origin;
    direction = lb.direction;

    return *this;
}

/*****************************************************************************
 
                              CPanel class
 
*****************************************************************************/

/** Default constructor */
CPanel::CPanel()
        : left(7), right(7), top(21), bottom(21),
        cutLeft(7), cutRight(7), cutTop(21), cutBottom(21),
        hasHems(false)
{}


/** Copy constructor.
 */
CPanel::CPanel( const CPanel &p )
        : label(p.label), left(p.left), right(p.right), top(p.top), bottom(p.bottom),
        cutLeft(p.cutLeft), cutRight(p.cutRight), cutTop(p.cutTop), cutBottom(p.cutBottom),
        hasHems(p.hasHems)
{}


/** This routine returns the smallest 3D box that contains the panel.
 */
CRect3d CPanel::boundingRect() const
{
    CRect3d rect;
    const CSide * sarray[4];
    if (hasHems)
    {
        sarray[0]=  &cutTop;
        sarray[1] = &cutBottom;
        sarray[2] = &cutLeft;
        sarray[3] = &cutRight;
        ;
    }
    else
    {
        sarray[0]=  &top;
        sarray[1] = &bottom;
        sarray[2] = &left;
        sarray[3] = &right;
        ;
    }

    rect.min = rect.max = top.point[0];

    for ( unsigned int s = 0 ; s < 4 ; s++ )
    {
        for ( unsigned int i = 0 ; i < sarray[s]->nbpoints() ; i++ )
        {
            CPoint3d curPoint = sarray[s]->point[i];
            for ( unsigned int j = 0 ; j < 3 ; j++ )
            {
                if ( curPoint[j] < rect.min[j] )
                    rect.min[j] = curPoint[j];
                if ( curPoint[j] > rect.max[j] )
                    rect.max[j] = curPoint[j];
            }
        }
    }

    return rect;
}

/** This routine returns the arithmetic average of all the edge points of a panel.
 */
CPoint3d CPanel::centroid() const
{
    const CSide * sarray[4] =
        {
            &top, &bottom, &left, &right
        };

    CPoint3d p, prev, sum;
    unsigned int  nbDiffPoints = 0;

    for ( unsigned int s = 0; s < 4; s++ )
    {
        for ( unsigned int i = 0 ; i < sarray[s]->nbpoints() ; i++ )
        {
            p = sarray[s]->point[i];
            if (( nbDiffPoints == 0) || ( p != prev ) )
            {
                // count only distinct points
                prev = p;
                sum = sum + p;
                nbDiffPoints++;
            }
        }
    }

    if ( nbDiffPoints > 0 )
    {
        real coeff = 1.0 / real(nbDiffPoints);
        sum = sum * coeff;
    }

    return sum;
}

/** This routine returns the height ( in Y direction ) of a panel
 */
real CPanel::height() const
{
    real h=0;

    real ymax = -111111, ymin = 111111;
    unsigned int i = 0;

    switch (hasHems)
    {
    case true:
        for ( i = 0 ; i < top.nbpoints() ; i++ )
        {
            if ( cutTop.point[i].y() > ymax )
                ymax = cutTop.point[i].y();
            if ( cutBottom.point[i].y() < ymin )
                ymin = cutBottom.point[i].y();
            //printf ("CUT pt i %d ymax %f ymin %f \n", i, ymax, ymin);
        }
    case false:
        for ( i = 0 ; i < top.nbpoints() ; i++ )
        {
            if ( top.point[i].y() > ymax )
                ymax = top.point[i].y();
            if ( bottom.point[i].y() < ymin )
                ymin = bottom.point[i].y();
            //printf ("BASE pt i %d ymax %f ymin %f \n", i, ymax, ymin);
        }
    }

    h = ymax - ymin;

    return h;
}


/** This routine returns the development of the panel.
 *  The developed panel will be horizontal with its upper or lower edge
 *  aligned to X axis depending on parameter "align"=ALIGN_TOP or ALIGN_BOTTOM
 *
 *  @author Robert Laine alias Sailcuter
 */
CPanel CPanel::develop(enumDevelopAlign align) const
{
    CPanel flatpanel;
    unsigned int npl = left.nbpoints();   // number of right/left points
    unsigned int npb = bottom.nbpoints(); // number of top/bottom points
    unsigned int i;
    real a = 0, b = 0, c = 0; // sides of triangle
    real CC = 0;  // angle opposite to side c of triangle
    CVector3d v;
    CPoint3d p1, p2, p3, p4;  // points for 2 triangular elements of panel to be developed
    CPoint3d d3(0,0,0);  // point after development

    /** establish origine at bottom point 0 */
    flatpanel.bottom.point[0] = d3;

    /** establish the baseline as (top point1 - bottom point1) */
    p1= bottom.point[1];
    p2 = top.point[1];
    p3 = bottom.point[0];
    a = CVector3d( p2 - p1 ).norm(); // vertical side at point 1
    b = CVector3d( p3 - p1 ).norm(); // lower side of triangle
    c = CVector3d( p2 - p3 ).norm();
    CC = Atriangle( a , b , c );      // angle of bottom left corner = opposite to side a

    d3.x() =  b;   // set base of first triangle on X axis
    flatpanel.bottom.point[1] = d3;

    // set point at top of baseline
    d3 = flatpanel.bottom.point[0] + CMatrix::rot3d( 2 , CC ) * CVector3d( 1 , 0 , 0 ) * c;
    flatpanel.top.point[1] = d3;

    // set baseline vector
    v = CVector3d( flatpanel.bottom.point[1] - flatpanel.top.point[1] );

    /** develop left side of panel by triangulation */
    for ( i = 0 ; i < npl ; i++ )
    {
        p3 = left.point[i];
        b = CVector3d( p3 - p1 ).norm(); // lower side of triangle
        c = CVector3d( p3 - p2 ).norm(); // upper side of triangle
        CC = Atriangle( c , a , b );      // angle of bottom corner = opposite to upper side
        // transpose corner of triangle in development plane
        d3 = flatpanel.bottom.point[1] + CMatrix::rot3d( 2 , -PI+CC ) * v * ( b/v.norm() );
        flatpanel.left.point[i] = d3;
    }

    // copy lower left point to bottom[0] to close the corner
    flatpanel.bottom.point[0] = flatpanel.left.point[0];
    // copy upper left point to top[0]
    flatpanel.top.point[0] = flatpanel.left.point[npl-1];

    /** develop body of panel by zig-zag triangulation */
    // reset baseline vector for lower right point
    v = CVector3d( flatpanel.bottom.point[1] - flatpanel.top.point[1] );
    c = v.norm();

    for ( i = 1 ; i < npb-2 ; i++ )
    {
        // define the 4 points corners of 2 adjacent triangles
        p1 = bottom.point[i];
        p2 = top.point[i];
        p3 = bottom.point[i+1];
        p4 = top.point[i+1];

        // first triangle = lower right p1-p2-p3
        a = c;
        b = CVector3d( p3 - p1 ).norm();
        c = CVector3d( p3 - p2 ).norm();
        CC = Atriangle( c , b , a );
        // transpose corner of triangle in development plane
        d3 = flatpanel.bottom.point[i] + CMatrix::rot3d( 2 , PI-CC )* v * ( b/v.norm() );
        flatpanel.bottom.point[i+1] = d3;

        // set baseline vector for upper right point
        v = CVector3d( flatpanel.top.point[i] - flatpanel.bottom.point[i+1] );

        // second triangle = upper right
        a = c;
        b = CVector3d( p4 - p2 ).norm();
        c = CVector3d( p4 - p3 ).norm();
        CC = Atriangle( c, b, a );
        // transpose corner of triangle in development plane
        d3 = flatpanel.top.point[i] + CMatrix::rot3d( 2 , -PI+CC )* v * ( b/v.norm() );
        flatpanel.top.point[i+1] = d3;

        // set next baseline vector
        v = CVector3d( flatpanel.bottom.point[i+1] - flatpanel.top.point[i+1] );
    }

    /** develop right side of panel */
    a= c;  // set baseline on last 2 points
    for ( i = 0 ; i < npl ; i++ )
    {
        p1 = right.point[i];
        b = CVector3d(p3-p1).norm(); // lower side of triangle
        c = CVector3d(p4-p1).norm(); // upper side of triangle
        CC = Atriangle(c,a,b);      // angle of bottom corner = opposite to upper side
        // transpose corner of triangle in development plane
        d3 = flatpanel.bottom.point[npb-2] + CMatrix::rot3d(2,PI-CC)* v *(b/v.norm());
        flatpanel.right.point[i]= d3;
    }

    // copy lower right point to bottom end to close the corner
    flatpanel.bottom.point[npb-1]= flatpanel.right.point[0];
    // copy upper right point to top end
    flatpanel.top.point[npb-1]= flatpanel.right.point[npl-1];

    /** re-align panel with top or bottom edge along X axis */
    CC = 0;
    if (align == ALIGN_TOP)
    { // align on top edge
        p1 = flatpanel.top.point[0];
        p2 = flatpanel.top.point[npb-1];
        CC = atan2( (p2.y()-p1.y()) , (p2.x()-p1.x()) );
    }
    else
    { // align on bottom edge
        p1 = flatpanel.bottom.point[0];
        p2 = flatpanel.bottom.point[npb-1];
        CC = atan2( (p2.y()-p1.y()) , (p2.x()-p1.x()) );
    }
    // align panel
    flatpanel = flatpanel.rotate(CPoint3d(0,0,0), CMatrix::rot3d(2,-CC) );

    flatpanel.hasHems = false;

    /** frame the developed panel to be X>0 and Y>0 */
    flatpanel = flatpanel.reframe(LOW_LEFT);

    return flatpanel;
}

/** Place the label at the center of the panel */
void CPanel::placeLabel()
{
    label.origin = centroid();
}


/** Reframe a panel such that the most left/right/top/bottom
 *  point is at coordinate X or Y =0
 */
CPanel CPanel::reframe(enumAlign align) const
{
    CPanel ret;
    unsigned int npl = left.nbpoints();   // number of right/left points
    unsigned int npb = bottom.nbpoints();  // number of top/bottom points
    unsigned int i;
    real xm=11111, ym=11111;

    switch ( align )
    {
    case LEFT:
        if ( hasHems == true )
        {
            for ( i = 0 ; i < npl ; i++ )
            {
                if ( cutRight.point[i].x() > xm )
                    xm = cutRight.point[i].x();
            }
        }
        else
        {
            for ( i = 0 ; i < npl ; i++ )
            {
                if ( right.point[i].x() > xm )
                    xm = right.point[i].x();
            }
        }

    case BOTTOM:
        if ( hasHems == true )
        {
            for ( i = 0 ; i < npb ; i++ )
            {
                if ( cutBottom.point[i].y() < ym )
                    ym = cutBottom.point[i].y();
            }
        }
        else
        {
            for ( i = 0; i < npb; i++)
            {
                if ( bottom.point[i].y() < ym )
                    ym = bottom.point[i].y();
            }
        }

    case LOW_LEFT:
        if ( hasHems ==  true )
        {
            for ( i = 0; i < npl; i++ )
            {
                if ( cutLeft.point[i].x() < xm )
                    xm = cutLeft.point[i].x();
            }

            for ( i = 0; i < npb; i++ )
            {
                if ( cutBottom.point[i].y() < ym )
                    ym = cutBottom.point[i].y();
            }
        }
        else
        {
            for ( i = 0; i < npl; i++ )
            {
                if ( left.point[i].x() < xm )
                    xm = left.point[i].x();
            }

            for ( i = 0; i < npb; i++ )
            {
                if ( bottom.point[i].y() < ym )
                    ym = bottom.point[i].y();
            }
        }
    }

    ret.left = left + CVector3d( -xm , -ym , 0 );
    ret.top = top + CVector3d( -xm , -ym , 0 );
    ret.right = right + CVector3d( -xm , -ym , 0 );
    ret.bottom = bottom + CVector3d( -xm , -ym , 0 );

    ret.cutLeft = cutLeft + CVector3d( -xm , -ym , 0 );
    ret.cutTop = cutTop + CVector3d( -xm , -ym , 0 );
    ret.cutRight = cutRight + CVector3d( -xm , -ym , 0 );
    ret.cutBottom = cutBottom + CVector3d( -xm , -ym , 0 );

    ret.hasHems = hasHems;

    return ret;
}

/** Add the cloth for stitching to the edges of the panel.
 *  This create the panel to be cut (outside the basic panel)
 *   lw = width to be added on left side
 *   tw = width to be added on top side
 *   rw = width to be added on right side
 *   bw = width to be added on bottom side
 *
 * @author Robert Laine alias Sailcuter
 */
void CPanel::addHems( const real &lw, const real &tw, const real &rw, const real &bw )
{
    hasHems = true;
    // printf(" hems lw= %f tw= %f rw= %f bw= %f \n" , lw, tw, rw, bw);
    CPoint3d pt(0,0,0);
    CVector3d v(1,0,0);
    CVector3d v0(1,0,0);
    CSubSpace Line1, Line2; // two lines 
    
    real minSize = 0.1;     // used to avoid computation near zero width side
    
    unsigned int i = 0;
    unsigned int npl = left.nbpoints(), npb = bottom.nbpoints();
    
    ///* compute basic edges vectors */
    CVector3d v1 = CVector3d( left.point[npl/2] - left.point[0] );
      //  if ( v1.norm() == 0 ) cout << "AddHems v1=0 " << endl;
    CVector3d v2 = CVector3d( left.point[npl-1] - left.point[npl/2] );
      //  if ( v2.norm() == 0 ) cout << "AddHems v2=0 " << endl;
    CVector3d v3 = CVector3d( right.point[npl/2] - right.point[0] );
      //  if ( v3.norm() == 0 ) cout << "AddHems v3=0 " << endl;
    CVector3d v4 = CVector3d( right.point[npl-1] - right.point[npl/2] );
      //  if ( v4.norm() == 0 ) cout << "AddHems v4=0 " << endl;
    CVector3d v5 = CVector3d( bottom.point[npb-1] - bottom.point[0] );
      //  if ( v5.norm() == 0 ) cout << "AddHems v5=0 " << endl;
    CVector3d v6 = CVector3d( bottom.point[npb-1] - bottom.point[npb-2] );
      //  if ( v6.norm() == 0 ) cout << "AddHems v6=0 " << endl;
    CVector3d v7 = CVector3d( top.point[npb-1] - top.point[0] );
      //  if ( v7.norm() == 0 ) cout << "AddHems v7=0 " << endl;
    CVector3d v8 = CVector3d( top.point[npb-1] - top.point[npb-2] );
      //  if ( v8.norm() == 0 ) cout << "AddHems v8=0 " << endl <<endl;

    ///* copy the basic panel edge points to cut points as default */
    for ( i = 0 ; i < npl ; i++ )
    {
        cutLeft.point[i] = left.point[i];
        cutRight.point[i] = right.point[i];
    }
    for ( i = 0 ; i < npb ; i++ )
    {
        cutTop.point[i] = top.point[i];
        cutBottom.point[i] = bottom.point[i];
    }
    
    if ( v5.norm() < minSize && v7.norm() < minSize )   // panel is a point
        throw CException ("CPanelLabel::addPanel : basic panel edges v5 and v6 too small");
    
    if (v5.norm() < minSize )
    {   // bottom side too small copy top
        v5 = v7.unit();
        v6 = v5;
    }
    else
    {   // reset v5 to bottom left point
        v5 = CVector3d( bottom.point[1] - bottom.point[0] );
    }
    
    if (v7.norm() < minSize )
    {   // top side too small copy bottom 
        v7 = v5.unit();
        v8 = v7;
    }
    else
    {   // reset v7 to top left point
        v7 = CVector3d( top.point[1] - top.point[0] );
    }
    
    ///* Move the basic bottom edge points to the cut line */
    if ( bw >= EPS )
    { // width of material is not too small
        for ( i = 0 ; i < npb ; i++ )
        {
            if ( i == 0 ) 
                v = CVector3d( bottom.point[1] - bottom.point[0] );
            else
                v = CVector3d( bottom.point[i] - bottom.point[i-1] );
            
            if ( v.norm() <= EPS ) 
                v = v5;

            cutBottom.point[i] = bottom.point[i] + CMatrix::rot3d(2,-PI/2) * v.unit() *bw;
        }
    }

    ///* Move the basic top edge points to the cut line */
    if ( tw >= EPS )
    { // width of material is not too small
        for ( i = 0 ; i < npb ; i++ )
        {
            if ( i == 0 )
                v = CVector3d( top.point[1] - top.point[0] );
            else
                v = CVector3d( top.point[i] - top.point[i-1] );
            
            if ( v.norm() <= minSize ) 
                v = v7;
            
            cutTop.point[i] = top.point[i] + CMatrix::rot3d(2,PI/2) * v.unit() *tw;
        }
    }

    ///* Move the basic left edge points to the cut line */
    if ( v1.norm() >= minSize )
    {  // lower left side is not a point 
        for ( i = 0 ; i < npl/2 ; i++)
        {
            if ( i == 0 )
                v = CVector3d( left.point[1] - left.point[0] );
            else
                v = CVector3d( left.point[i] - left.point[i-1] );

            cutLeft.point[i] = left.point[i] + CMatrix::rot3d( 2 , PI/2) * v.unit() * lw;
        }
        
        v0 = CVector3d( left.point[npl/2] - left.point[npl/2 -1] );
        Line1 = CSubSpace3d::line( cutLeft.point[npl/2 -1] , v0 );

        if ( v2.norm() >= minSize )
        {  // upper left side is not a point
            for ( i = npl/2 +1 ; i < npl ; i++ )
            {
                v2 = CVector3d( left.point[i] - left.point[i-1] );
                cutLeft.point[i] = left.point[i] + CMatrix::rot3d( 2 , PI/2) * v2.unit() * lw;
                
                if ( i == npl/2 +1 )
                {   // compute mid side break point
                    Line2 = CSubSpace3d::line( cutLeft.point[i] , v2 );
                    
                    if ( Line1.intersect(Line2).getdim() == 0 )
                        cutLeft.point[npl/2] = Line1.intersect(Line2).getp();
                    else throw "ERROR in CPanel::addHems = no mid left side intersection point";
                    
                    // check adjacent points relative to mid side point
                    if ( (CVector3d(cutLeft.point[npl/2] - cutLeft.point[npl/2 -1]) * v0) <= 0 )
                        cutLeft.point[npl/2 -1] = cutLeft.point[npl/2 -2];
                    if ( (CVector3d(cutLeft.point[npl/2 +1] - cutLeft.point[npl/2]) * v0) <= 0 )
                        cutLeft.point[npl/2 +1] = cutLeft.point[npl/2 +2];
                }
                else
                {   // compute other points
                    cutLeft.point[i] = left.point[i] + CMatrix::rot3d( 2 , PI/2) * v2.unit() * lw;
                    // check position relative to mid side point                
                    if ( (CVector3d(cutLeft.point[i] - cutLeft.point[npl/2]) * v0) <= 0 )
                        cutLeft.point[npl/2 +1] = cutLeft.point[npl/2];
                }
            }
        }
        else
        { // upper left side is a point but not lower side
            v2 = CVector3d( left.point[npl/2] - left.point[npl/2 -1] );
            for ( i = npl/2 +1 ; i < npl ; i++ )
                cutLeft.point[i] = left.point[i] + CMatrix::rot3d( 2 , PI/2) * v2.unit() * lw;
        }
    }
    else if ( v2.norm() >= minSize )
    {  // only lower left side is a point
        v1 = CVector3d( left.point[npl/2 +1] - left.point[npl/2] );
        for ( i = 0 ; i < npl/2  ; i++ )
            cutLeft.point[i] = left.point[i] + CMatrix::rot3d( 2 , PI/2) * v1.unit() * lw;

        for ( i = npl/2 +1 ; i < npl -1 ; i++ )
        {
            v2 = CVector3d( left.point[i] - left.point[i-1] );
            cutLeft.point[i] = left.point[i] + CMatrix::rot3d( 2 , PI/2) * v2.unit() * lw;
        }
    }
    else
    {  // complete left side is a point
        if ( bw == 0 )
            v = -v5; // extend the bottom edge
        else
            v = -( v5.unit() + v7.unit() ); // extend in bissectrice top-bottom

        for ( i = 0 ; i< npl ; i++)
            cutLeft.point[i] = left.point[i] + v.unit() * lw;
            
        // if (v.norm() == 0) cout << "AddHems 10 v=0 v5="<< v5.norm()<< " v7="<< v7.norm()<< endl;
        v1 = CMatrix::rot3d(2,-PI/2) * v.unit();
        v2 = v1;
    }

    ///* Move the basic right edge points to the cut line */
    if ( v3.norm() >= minSize )
    {  // lower right side is not a point 
        for ( i = 0 ; i < npl/2 ; i++)
        {
            if ( i == 0 )
            {
                if ( right.point[i+1] != right.point[i] )
                    v = CVector3d( right.point[i+1] - right.point[i] );
                else if ( right.point[i+2] != right.point[i] )
                    v = CVector3d( right.point[i+2] - right.point[i] );
                else if ( right.point[i+3] != right.point[i] )
                    v = CVector3d( right.point[i+3] - right.point[i] );
                else if ( right.point[i+4] != right.point[i] )
                    v = CVector3d( right.point[i+4] - right.point[i] );
            }
            else
            {
                if ( right.point[i] != right.point[i-1] )
                    v = CVector3d( right.point[i] - right.point[i-1] );
                else if ( right.point[i+1] != right.point[i] )
                    v = CVector3d( right.point[i+1] - right.point[i] );
                else if ( right.point[i+2] != right.point[i] )
                    v = CVector3d( right.point[i+2] - right.point[i] );
            }
 
            cutRight.point[i] = right.point[i] + CMatrix::rot3d( 2 , -PI/2) * v.unit() * rw;
        }
        
        v0 = CVector3d( right.point[npl/2] - right.point[npl/2 -1] );
        Line1 = CSubSpace3d::line( cutRight.point[npl/2 -1] , v0 );

        if ( v4.norm() >= minSize )
        {  // upper right side is not a point
            for ( i = npl/2 +1 ; i < npl ; i++ )
            {
                v4 = CVector3d( right.point[i] - right.point[i-1] );
                cutRight.point[i] = right.point[i] + CMatrix::rot3d( 2 , -PI/2) * v4.unit() * rw;
                if ( i == npl/2 +1 )
                {   // compute mid side break point
                    Line2 = CSubSpace3d::line( cutRight.point[i] , v4 );
                    
                    if (Line1.intersect(Line2).getdim() == 0)
                        cutRight.point[npl/2] = Line1.intersect(Line2).getp();
                    else throw "ERROR in CPanel::addHems = no mid right side intersection point";
                    
                    // check adjacent points relative to mid side point
                    if ( (CVector3d(cutRight.point[npl/2] - cutRight.point[npl/2 -1]) * v0) <= 0 )
                        cutRight.point[npl/2 -1] = cutRight.point[npl/2 -2];
                    if ( (CVector3d(cutRight.point[npl/2 +1] - cutRight.point[npl/2]) * v0) <= 0 )
                        cutRight.point[npl/2 +1] = cutRight.point[npl/2 +2];
                }
                else
                {   // compute other points
                    cutRight.point[i] = right.point[i] + CMatrix::rot3d( 2 , -PI/2) * v4.unit() * rw;
                }
            }
        }
        else
        { // upper right side is a point but not lower side
            v4 = CVector3d( right.point[npl-1] - right.point[npl-2] );
 
            for ( i = npl/2 +1 ; i < npl ; i++ )
                cutRight.point[i] = right.point[i] + CMatrix::rot3d( 2 , -PI/2) * v4.unit() * rw;
        }
    }
    else if ( v4.norm() >= minSize )
    {   // only lower right side is a point
        v3 = v4;
        //v3 = CVector3d( right.point[npl-2] - right.point[npl/2] );
/*        if (v3.norm() == 0) 
        {   
                cout << "AddHems v3=0 : about to crash 13" << endl;
            for ( i = 0 ; i < npl ; i++ )
                cout << "pt " << i << " xyz= " << right.point[i] << endl;
        }
*/      
        for ( i = 0 ; i < npl/2  ; i++ )
            cutRight.point[i] = right.point[i] + CMatrix::rot3d( 2 , -PI/2) * v4.unit() * rw;

        for ( i = npl/2 +1 ; i < npl ; i++ )
        {
            cutRight.point[i] = right.point[i] + CMatrix::rot3d( 2 , -PI/2) * v4.unit() * rw;
        }
        v4 = CVector3d( right.point[npl-1] - right.point[npl-2] );
          //    if (v4.norm() == 0) cout << "AddHems v4=0 about to crash 13" << endl;
    }
    else
    {  // complete right side is a point
        if ( bw == 0 )
            v = v6;
        else
            v = ( v6 + v8 );

            if (v.norm() == 0) cout << "AddHems v=v6=0 or v=v6+v8=0 about to crash 14" << endl;
        
        for ( i = 0 ; i< npl ; i++)
            cutRight.point[i] = right.point[i] + v.unit() * rw;

        v3 = CMatrix::rot3d(2 , PI/2) * v.unit();
        v4 = v3;
    }

    ///* Rejoining the 4 corners of the cut panel */
    /// lower left
    if (v5.norm() == 0) 
        cout << "AddHems v5=0 about to crash 15" << endl;
    Line1 = CSubSpace3d::line( cutBottom.point[0] , v5 );
    
    if (v6.norm() == 0) 
        cout << "AddHems v6=0 about to crash 16" << endl;
    Line2 = CSubSpace3d::line( cutLeft.point[0] , v1 );
    
    if (Line1.intersect(Line2).getdim() == 0)
        pt = Line1.intersect(Line2).getp();
    else throw "ERROR in CPanel::addHems = rejoining lower corner no left intersection point";

    /* Adjust the lower left point [0] to be at intersection */
    cutBottom.point[0] = pt;
    cutLeft.point[0] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the point pt
     */
    for ( i = 0 ; i < npl/2 ; i++ )
    {
        if ( (CVector3d( cutLeft.point[i] - pt) * v1 ) <= 0 )
            cutLeft.point[i] = pt;

        if ( (CVector3d( cutBottom.point[i] - pt) * v5 ) <= 0 )
            cutBottom.point[i] = pt;
    }
    
    /// lower right
    Line1 = CSubSpace3d::line( cutBottom.point[npb-1] , v6 );
    Line2 = CSubSpace3d::line( cutRight.point[0] , v3 );
 
    if (Line1.intersect(Line2).getdim() == 0)
        pt = Line1.intersect(Line2).getp();
    else throw "ERROR in CPanel::addHems = rejoining lower corner no right intersection point"; 

    /* Adjust the lower left point [0] to be at intersection */
    cutBottom.point[npb-1] = pt;
    cutRight.point[0] = pt;
    
    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the point pt
     */
    for ( i = 1 ; i < npl/2 ; i++ )
    {
        if ( (CVector3d( cutRight.point[i] - pt) * v3 ) <= 0 )
            cutRight.point[i] = pt;
        if ( (CVector3d( cutBottom.point[npb-1-i] - pt) * v6 ) >= 0 )
            cutBottom.point[npb-1-i] = pt;
    }

    /// upper left
    Line1 = CSubSpace3d::line( cutTop.point[0] , v7 );
    Line2 = CSubSpace3d::line( cutLeft.point[npl-1] , v2 );
    
    if (Line1.intersect(Line2).getdim() == 0)
        pt = Line1.intersect(Line2).getp();
    else throw "ERROR in CPanel::addHems = rejoining upper corner no left intersection point";

    /* Adjust the upper left point to be at intersection */
    cutTop.point[0] = pt;
    cutLeft.point[npl-1] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the intersect point pt
     */
    for ( i = 0 ; i < npl/2 ; i++ )
    {
        if ( (CVector3d( cutLeft.point[npl -1 -i] - pt) * v2 ) >= 0 )
            cutLeft.point[npl-1-i] = pt;
        if ( (CVector3d( cutTop.point[i] - pt) * v7 ) <= 0 )
            cutTop.point[i] = pt;
    }

    /// upper right
    Line1 = CSubSpace3d::line( cutTop.point[npb-1] , v8 );
    Line2 = CSubSpace3d::line( cutRight.point[npl-1] , v4 );
    
    if (Line1.intersect(Line2).getdim() == 0)
        pt = Line1.intersect(Line2).getp();
    else throw "ERROR in CPanel::addHems = rejoining upper corner no right intersection point";
    
    /* Adjust the upper right point to be at intersection */
        cutTop.point[npb-1] = pt;
        cutRight.point[npl-1] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the intersect point pt
     */
    for ( i = 1 ; i < npl/2 ; i++ )
    {
        if ( (CVector3d( cutRight.point[npl-1-i] - pt) * v4 ) >= 0 )
            cutRight.point[npl-1-i] = pt;

        if ( (CVector3d(cutTop.point[npb-1-i] - pt) * v6) >= 0 )
            cutTop.point[npb-1-i] = pt;
    }
        
} /// end addHems ////////////////////////////////////////


/** Rotates a panel around a point.
 *  p = centre of rotation
 *  m = rotation matrix
 */
CPanel CPanel::rotate( const CPoint3d &p, const CMatrix &m ) const
{
    CPanel panel;
    panel.hasHems = hasHems;

    panel.label = label.rotate( p , m );

    panel.left = left.rotate( p , m );
    panel.right = right.rotate( p , m );
    panel.top = top.rotate( p , m );
    panel.bottom = bottom.rotate( p , m );

    panel.cutLeft = cutLeft.rotate( p , m );
    panel.cutRight = cutRight.rotate( p , m );
    panel.cutTop = cutTop.rotate( p , m );
    panel.cutBottom = cutBottom.rotate( p , m );

    return panel;
}

// operators

/** Performs a 3D translation of the panel by a given vector.
 */
CPanel CPanel::operator+ (const CVector3d &transl) const
{
    CPanel ret;
    ret.hasHems = hasHems;

    ret.label = label+transl;

    ret.left = left + transl;
    ret.right = right + transl;
    ret.top = top + transl;
    ret.bottom = bottom + transl;

    ret.cutLeft = cutLeft + transl;
    ret.cutRight = cutRight + transl;
    ret.cutTop = cutTop + transl;
    ret.cutBottom = cutBottom + transl;

    return ret;
}


/** Performs an assignment.
 */
CPanel& CPanel::operator= (const CPanel &p)
{
    if (&p == this)
        return *this;

    label = p.label;

    left = p.left;
    right = p.right;
    top = p.top;
    bottom = p.bottom;

    cutLeft = p.cutLeft;
    cutRight = p.cutRight;
    cutTop = p.cutTop;
    cutBottom = p.cutBottom;

    hasHems = p.hasHems;

    return *this;
}


/*****************************************************************************
 
                              CSide class
 
*****************************************************************************/

/** Constructs a side with a given number of points.
 */
CSide::CSide( unsigned int nbpoints /* = 1 */)
{
    point.resize(nbpoints);
}


/** Copy constructor.
 */
CSide::CSide( const CSide &s )
{
    point = s.point;
}


/** Makes the side a straight line between two points.
 */
void CSide::fill( const CPoint3d &p1 , const CPoint3d &p2 )
{
    switch ( point.size() )
    {
    case 0:
        return;
    case 1:
        point[0] = p1;
        return;
    }

    for( unsigned int i = 0 ; i < point.size() ; i++ )
        point[i] = p1 + ( p2 - p1 ) * ( real(i) / (point.size() -1) );
}


/** Makes the side a two-segment line between three points.
 */
void CSide::fill( const CPoint3d &p1 , const CPoint3d &p2 , const CPoint3d &p3 )
{
    switch ( point.size() )
    {
    case 0:
        return;
    case 1:
        point[0] = p1;
        return;
    case 2:
        point[0] = p1;
        point[1] = p3;
        return;
    }

    unsigned int n1  = int( point.size() ) / 2;
    for( unsigned int i = 0 ; i < point.size() ; i++ )
    {
        if ( i <= n1 )
            point[i] = p1 + (p2 - p1) * (real(i) / n1);
        else
            point[i] = p2 + (p3 - p2) * (real(i - n1) / (point.size() -n1 -1) );
    }
}


/** Rotates a CSide around a point.
 */
CSide CSide::rotate( const CPoint3d &p, const CMatrix &m ) const
{
    CSide s( point.size() );

    for ( unsigned int i = 0 ; i < point.size() ; i++ )
        s.point[i] = p + m * (point[i] - p);

    return s;
}

// operators

/** Performs a 3D translation of the side by a given vector.
 */
CSide CSide::operator+ (const CVector3d &transl) const
{
    CSide ret( point.size() );
    for ( unsigned int i = 0 ; i < point.size() ; i++ )
        ret.point[i] = transl + point[i];
    return ret;
}


/** Performs an assigment.
 */
CSide& CSide::operator=(const CSide& s)
{
    if (&s == this)
        return *this;

    point = s.point;

    return *this;
}


/*********************************************
 
            Global functions
 
 *********************************************/

/** Outputs a CPanel  to a stream.
 *  exemple:  cout << panel[i];
 */
ostream& operator<<(ostream& o , const CPanel &p)
{
    o << p.label;
    o << "== CSide : left ==" << endl << p.left;
    o << "== CSide : top ==" << endl << p.top;
    o << "== CSide : right ==" << endl << p.right;
    o << "== CSide : bottom ==" << endl << p.bottom;

    if ( p.hasHems )
    {
        o << "== CSide : cutLeft ==" << endl << p.cutLeft;
        o << "== CSide : cutTop ==" << endl << p.cutTop;
        o << "== CSide : cutRight ==" << endl << p.cutRight;
        o << "== CSide : cutBottom ==" << endl << p.cutBottom;
    }
    return o;
}

/** Outputs a CPanelLabel  to a stream.
 *  exemple:  cout << panel[i].label;
 */
ostream& operator<< (ostream &o , const CPanelLabel &lb)
{
    o << "== CPanelLabel : name ==" << endl << string(lb.name.toLocal8Bit()) << endl;
    o << "== CPanelLabel : height ==" << endl << lb.height << endl;
    o << "== CPanelLabel : color ==" << endl << lb.color << endl;
    o << "== CPanelLabel : origin ==" << endl << lb.origin << endl;
    o << "== CPanelLabel : direction ==" << endl << lb.direction << endl;
    return o;
}

/** Outputs a CSide  to a stream.
 *  exemple:  cout << panel[i].left;
 */
ostream& operator<< (ostream &out, const CSide &s)
{
    for ( unsigned int i = 0 ; i < s.nbpoints() ; i++ )
    {
        out << "#" << i << "\t" << s.point[i] << endl;
    }
    return out;
}

