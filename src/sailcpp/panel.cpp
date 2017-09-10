/*
 * Copyright (C) 1993-2017 Robert & Jeremy Laine
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

#include "panel.h"
#include "sailcalc.h"

/** Normalize the given vector then rotate it about the Z axis.
 */
CVector3d rotateNormalized(real angle, const CVector3d &v)
{
    CMatrix4x4 matrix;
    matrix.rotate(angle, CVector3d(0, 0, 1));
    return matrix * v.normalized();
}

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


/** Transforms a label.
 */
CPanelLabel CPanelLabel::transformed(const CMatrix4x4 &m) const
{
    CPanelLabel lb;

    lb.name = name;
    lb.height = height;
    lb.color = color;
    lb.origin = m * origin;
    lb.direction = m * direction - m * CVector3d(0, 0, 0);

    return lb;
}

// operators

/** Performs an assignment.
 */
CPanelLabel& CPanelLabel::operator= (const CPanelLabel &lb)
{
    if (&lb == this)
        return *this;

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

    rect.min = rect.max = top[0];

    for (unsigned int s = 0 ; s < 4 ; s++)
    {
        for (unsigned int i = 0 ; i < sarray[s]->size() ; i++)
        {
            CPoint3d curPoint = (*sarray[s])[i];
            for (unsigned int j = 0 ; j < 3 ; j++)
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

    for (unsigned int s = 0; s < 4; s++)
    {
        for (unsigned int i = 0 ; i < sarray[s]->size() ; i++)
        {
            p = (*sarray[s])[i];
            if (( nbDiffPoints == 0) || !qFuzzyCompare(p, prev) )
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


/** This routine returns the development of the panel.
 *  The developed panel will be horizontal with its upper or lower edge
 *  aligned to X axis depending on parameter "align"=ALIGN_TOP or ALIGN_BOTTOM
 *
 *  @author Robert Laine alias Sailcuter
 */
CPanel CPanel::develop(enumDevelopAlign align) const
{
    CPanel flatpanel;
    unsigned int npl = left.size();   // number of right/left points
    unsigned int npb = bottom.size(); // number of top/bottom points
    unsigned int i;
    real a = 0, b = 0, c = 0; // sides of triangle
    real CC = 0;  // angle opposite to side c of triangle
    CVector3d v;
    CPoint3d p1, p2, p3, p4;  // points for 2 triangular elements of panel to be developed
    CPoint3d d3(0,0,0);  // point after development

    /** establish origine at bottom point 0 */
    flatpanel.bottom[0] = d3;

    /** establish the baseline as (top point1 - bottom point1) */
    p1= bottom[1];
    p2 = top[1];
    p3 = bottom[0];
    a = ( p2 - p1 ).length(); // vertical side at point 1
    b = ( p3 - p1 ).length(); // lower side of triangle
    c = ( p2 - p3 ).length();
    CC = Atriangle( a , b , c );      // angle of bottom left corner = opposite to side a

    d3.setX(b);   // set base of first triangle on X axis
    flatpanel.bottom[1] = d3;

    // set point at top of baseline
    d3 = flatpanel.bottom[0] + rotateNormalized(CC, CVector3d(1, 0, 0)) * c;
    flatpanel.top[1] = d3;

    // set baseline vector
    v = flatpanel.bottom[1] - flatpanel.top[1];

    /** develop left side of panel by triangulation */
    for (i = 0 ; i < npl ; i++)
    {
        p3 = left[i];
        b = ( p3 - p1 ).length(); // lower side of triangle
        c = ( p3 - p2 ).length(); // upper side of triangle
        CC = Atriangle( c , a , b );      // angle of bottom corner = opposite to upper side
        // transpose corner of triangle in development plane
        d3 = flatpanel.bottom[1] + rotateNormalized(-M_PI + CC, v) * b;
        flatpanel.left[i] = d3;
    }

    // copy lower left point to bottom[0] to close the corner
    flatpanel.bottom[0] = flatpanel.left[0];
    // copy upper left point to top[0]
    flatpanel.top[0] = flatpanel.left[npl-1];

    /** develop body of panel by zig-zag triangulation */
    // reset baseline vector for lower right point
    v = flatpanel.bottom[1] - flatpanel.top[1];
    c = v.length();

    for (i = 1 ; i < npb-2 ; i++)
    {
        // define the 4 points corners of 2 adjacent triangles
        p1 = bottom[i];
        p2 = top[i];
        p3 = bottom[i+1];
        p4 = top[i+1];

        // first triangle = lower right p1-p2-p3
        a = c;
        b = ( p3 - p1 ).length();
        c = ( p3 - p2 ).length();
        CC = Atriangle( c , b , a );
        // transpose corner of triangle in development plane
        d3 = flatpanel.bottom[i] + rotateNormalized(M_PI - CC, v) * b;
        flatpanel.bottom[i+1] = d3;

        // set baseline vector for upper right point
        v = flatpanel.top[i] - flatpanel.bottom[i+1];

        // second triangle = upper right
        a = c;
        b = ( p4 - p2 ).length();
        c = ( p4 - p3 ).length();
        CC = Atriangle( c, b, a );
        // transpose corner of triangle in development plane
        d3 = flatpanel.top[i] + rotateNormalized(-M_PI + CC, v) * b;
        flatpanel.top[i+1] = d3;

        // set next baseline vector
        v = flatpanel.bottom[i+1] - flatpanel.top[i+1];
    }

    /** develop right side of panel */
    a= c;  // set baseline on last 2 points
    for (i = 0 ; i < npl ; i++)
    {
        p1 = right[i];
        b = CVector3d(p3-p1).length(); // lower side of triangle
        c = CVector3d(p4-p1).length(); // upper side of triangle
        CC = Atriangle(c,a,b);      // angle of bottom corner = opposite to upper side
        // transpose corner of triangle in development plane
        d3 = flatpanel.bottom[npb-2] + rotateNormalized(M_PI - CC, v) * b;
        flatpanel.right[i]= d3;
    }

    // copy lower right point to bottom end to close the corner
    flatpanel.bottom[npb-1]= flatpanel.right[0];
    // copy upper right point to top end
    flatpanel.top[npb-1]= flatpanel.right[npl-1];

    /** re-align panel with top or bottom edge along X axis */
    CC = 0;
    if (align == ALIGN_TOP)
    { // align on top edge
        p1 = flatpanel.top[0];
        p2 = flatpanel.top[npb-1];
        CC = atan2( (p2.y()-p1.y()) , (p2.x()-p1.x()) );
    }
    else
    { // align on bottom edge
        p1 = flatpanel.bottom[0];
        p2 = flatpanel.bottom[npb-1];
        CC = atan2( (p2.y()-p1.y()) , (p2.x()-p1.x()) );
    }
    // align panel
    flatpanel = flatpanel.rotated(CPoint3d(0,0,0), -CC, Qt::ZAxis);

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
    unsigned int npl = left.size();   // number of right/left points
    unsigned int npb = bottom.size();  // number of top/bottom points
    unsigned int i;
    real xm=11111, ym=11111;

    switch ( align )
    {
    case LEFT:
        if ( hasHems == true )
        {
            for (i = 0 ; i < npl ; i++)
            {
                if ( cutRight[i].x() > xm )
                    xm = cutRight[i].x();
            }
        }
        else
        {
            for (i = 0 ; i < npl ; i++)
            {
                if ( right[i].x() > xm )
                    xm = right[i].x();
            }
        }

    case BOTTOM:
        if ( hasHems == true )
        {
            for (i = 0 ; i < npb ; i++)
            {
                if ( cutBottom[i].y() < ym )
                    ym = cutBottom[i].y();
            }
        }
        else
        {
            for (i = 0; i < npb; i++)
            {
                if ( bottom[i].y() < ym )
                    ym = bottom[i].y();
            }
        }

    case LOW_LEFT:
        if ( hasHems ==  true )
        {
            for (i = 0; i < npl; i++)
            {
                if ( cutLeft[i].x() < xm )
                    xm = cutLeft[i].x();
            }

            for (i = 0; i < npb; i++)
            {
                if ( cutBottom[i].y() < ym )
                    ym = cutBottom[i].y();
            }
        }
        else
        {
            for (i = 0; i < npl; i++)
            {
                if ( left[i].x() < xm )
                    xm = left[i].x();
            }

            for (i = 0; i < npb; i++)
            {
                if ( bottom[i].y() < ym )
                    ym = bottom[i].y();
            }
        }
    }

    return (*this + CVector3d( -xm , -ym , 0 ));
}
/** Add the cloth for stitching to the 4 edges of the panel.
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
    add6Hems( lw, lw, tw, rw, rw, bw );
}

/** Add the cloth for stitching to the 6 edges of the panel.
 *  This create the panel to be cut (outside the basic panel)
 *   lolW, hilW = width to be added on lo-hi left side
 *   topW = width to be added on top side
 *   lorW, hirW = width to be added on lo-hi right side
 *   botW = width to be added on bottom side
 *
 * @author Robert Laine alias Sailcuter
 */

void CPanel::add6Hems( const real &lolW, const real &hilW, const real &topW, const real &hirW, const real &lorW, const real &botW )
{
    hasHems = true;
    CPoint3d pt(0,0,0);
    CVector3d v(1,0,0);
    CVector3d v0(1,0,0);
    CSubSpace Line1, Line2; // two lines

    real minSize = 0.1;     // used to avoid computation near zero width side

    unsigned int i = 0;
    unsigned int npl = left.size(), npb = bottom.size();

    ///* compute basic edges vectors */
    CVector3d v1 = left[npl/2] - left[0];
    //  if ( v1.length() == 0 ) cout << "CPanel::add6Hems v1=0 " << endl;
    CVector3d v2 = left[npl-1] - left[npl/2];
    //  if ( v2.length() == 0 ) cout << "CPanel::add6Hems v2=0 " << endl;
    CVector3d v3 = right[npl/2] - right[0];
    //  if ( v3.length() == 0 ) cout << "CPanel::add6Hems v3=0 " << endl;
    CVector3d v4 = right[npl-1] - right[npl/2];
    //  if ( v4.length() == 0 ) cout << "CPanel::add6Hems v4=0 " << endl;
    CVector3d v5 = bottom[npb-1] - bottom[0];
    //  if ( v5.length() == 0 ) cout << "CPanel::add6Hems v5=0 " << endl;
    CVector3d v6 = bottom[npb-1] - bottom[npb-2];
    //  if ( v6.length() == 0 ) cout << "CPanel::add6Hems v6=0 " << endl;
    CVector3d v7 = top[npb-1] - top[0];
    //  if ( v7.length() == 0 ) cout << "CPanel::add6Hems v7=0 " << endl;
    CVector3d v8 = top[npb-1] - top[npb-2];
    //  if ( v8.length() == 0 ) cout << "CPanel::add6Hems v8=0 " << endl <<endl;

    //cout << "CPanel::add6Hems start" << endl;
    //cout << "   lolW=" << lolW << " hilW=" << hilW << "   topW =" << topW <<  endl;
    //cout << "   lorW=" << lorW << " hirW=" << hirW << "   botW=" << botW << endl;

    ///* copy the basic panel edge points to cut points as default */
    for (i = 0 ; i < npl ; i++) {
        cutLeft[i] = left[i];
        cutRight[i] = right[i];
    }

    for (i = 0 ; i < npb ; i++) {
        cutTop[i] = top[i];
        cutBottom[i] = bottom[i];
    }

    if ( v5.length() < minSize && v7.length() < minSize )   // panel is a point
        throw panel_error("CPanel::add6Hems : bottom panel edges v5 and v6 too small");

    if (v5.length() < minSize ) {   // bottom side too small copy top
        v5 = v7.normalized();
        v6 = v5;
    }
    else {   // reset v5 to bottom left point
        v5 = bottom[1] - bottom[0];
    }

    if (v7.length() < minSize ) {   // top side too small copy bottom
        v7 = v5.normalized();
        v8 = v7;
    }
    else {   // reset v7 to top left point
        v7 = top[1] - top[0];
    }

    ///* Move the basic bottom edge points to the cut line */
    //cout << "CPanel::add6Hems move basic bottom edge" << endl;

    if ( botW > EPS ) { // width of material is not too small
        for (i = 0 ; i < npb ; i++) {
            if ( i == 0 )
                v = bottom[1] - bottom[0];
            else
                v = bottom[i] - bottom[i-1];

            if ( v.length() <= EPS )
                v = v5;

            cutBottom[i] = bottom[i] + rotateNormalized(-M_PI / 2, v) * botW;
        }
    }

    ///* Move the basic top edge points to the cut line */
    if ( topW > EPS ) { // width of material is not too small
        for (i = 0 ; i < npb ; i++) {
            if ( i == 0 )
                v = top[1] - top[0];
            else
                v = top[i] - top[i-1];

            if ( v.length() <= minSize )
                v = v7;

            cutTop[i] = top[i] + rotateNormalized(M_PI / 2, v) * topW;
        }
    }

    ///* Move the basic left edge points to the cut line */
    if ( v1.length() >= minSize ) {  // lower left side is not a point
        for (i = 0 ; i < npl/2 ; i++) {
            if ( i == 0 )
                v = left[1] - left[0];
            else
                v = left[i] - left[i-1];

            cutLeft[i] = left[i] + rotateNormalized(M_PI/2, v) * lolW;
        }

        v0 = left[npl/2] - left[npl/2 -1];
        Line1 = CSubSpace::line( cutLeft[npl/2 -1] , v0 );

        if ( v2.length() >= minSize ) {  // upper left side is not a point
            for (i = npl/2 +1 ; i < npl ; i++) {
                v2 = left[i] - left[i-1];
                cutLeft[i] = left[i] + rotateNormalized(M_PI/2, v2) * hilW;

                if ( i == npl/2 +1 ) {   // compute mid side break point
                    Line2 = CSubSpace::line( cutLeft[i] , v2 );

                    if ( Line1.intersect(Line2).getdim() == 0 )
                        cutLeft[npl/2] = Line1.intersect(Line2).getp();
                    // else cout << "CPanel::add6Hems = no mid left side intersection point" << endl;

                    // check adjacent points relative to mid side point
                    if (CVector3d::dotProduct(cutLeft[npl/2] - cutLeft[npl/2 -1], v0) <= 0)
                        cutLeft[npl/2 -1] = cutLeft[npl/2 -2];
                    if (CVector3d::dotProduct(cutLeft[npl/2 +1] - cutLeft[npl/2], v0) <= 0)
                        cutLeft[npl/2 +1] = cutLeft[npl/2 +2];
                }
                else {   // compute other uppr lft points
                    cutLeft[i] = left[i] + rotateNormalized(M_PI/2, v2) * hilW;
                    // check position relative to mid side point
                    if (CVector3d::dotProduct(cutLeft[i] - cutLeft[npl/2], v0) <= 0)
                        cutLeft[npl/2 +1] = cutLeft[npl/2];
                }
            }
        }
        else {  // upper left side is a point but not lower side
            v2 = left[npl/2] - left[npl/2 -1];
            for (i = npl/2 +1 ; i < npl ; i++)
                cutLeft[i] = left[i] + rotateNormalized(M_PI/2, v2) * hilW;
        }
    }
    else if ( v2.length() >= minSize ) {
        // only lower left side is a point
        v1 = left[npl/2 +1] - left[npl/2];
        for (i = 0 ; i < npl/2  ; i++)
            cutLeft[i] = left[i] + rotateNormalized(M_PI/2, v1) * lolW;

        for (i = npl/2 +1 ; i < npl -1 ; i++)
        {
            v2 = left[i] - left[i-1];
            cutLeft[i] = left[i] + rotateNormalized(M_PI/2, v2) * hilW;
        }
    }
    else {
        // complete left side is a point
        if ( botW == 0 )
            v = -v5; // extend the bottom edge
        else
            v = -( v5.normalized() + v7.normalized() ); // extend in bissectrice top-bottom

        for (i = 0 ; i< npl ; i++)
            cutLeft[i] = left[i] + v.normalized() * lolW;

        // if (v.length() == 0) cout << "CPanel::add6Hems 10 v=0 v5="<< v5.length()<< " v7="<< v7.length()<< endl;
        v1 = rotateNormalized(-M_PI/2, v);
        v2 = v1;
    }

    ///* Move the basic right edge points to the cut line */
    if ( v3.length() >= minSize ) {  // lower right side is not a point
        for (i = 0 ; i < npl/2 ; i++) {
            if ( i == 0 ) {
                if (!qFuzzyCompare(right[i+1], right[i]))
                    v = right[i+1] - right[i];
                else if (!qFuzzyCompare(right[i+2], right[i]))
                    v = right[i+2] - right[i];
                else if (!qFuzzyCompare(right[i+3], right[i]))
                    v = right[i+3] - right[i];
                else if (!qFuzzyCompare(right[i+4], right[i]))
                    v = right[i+4] - right[i];
            }
            else {
                if (!qFuzzyCompare(right[i], right[i-1]))
                    v = right[i] - right[i-1];
                else if (!qFuzzyCompare(right[i+1], right[i]))
                    v = right[i+1] - right[i];
                else if (!qFuzzyCompare(right[i+2], right[i]))
                    v = right[i+2] - right[i];
            }
            cutRight[i] = right[i] + rotateNormalized(-M_PI/2, v) * lorW;
        }

        v0 = right[npl/2] - right[npl/2 -1];
        Line1 = CSubSpace::line( cutRight[npl/2 -1] , v0 );

        if ( v4.length() >= minSize ) {  // upper right side is not a point
            for (i = npl/2 +1 ; i < npl ; i++) {
                v4 = right[i] - right[i-1];
                cutRight[i] = right[i] + rotateNormalized(-M_PI/2, v4) * hirW;
                if ( i == npl/2 +1 ) {   // compute mid side break point
                    Line2 = CSubSpace::line( cutRight[i] , v4 );

                    if (Line1.intersect(Line2).getdim() == 0)
                        cutRight[npl/2] = Line1.intersect(Line2).getp();
                    // else cout << "CPanel::add6Hems = no mid right side intersection point" << endl;

                    // check adjacent points relative to mid side point
                    if (CVector3d::dotProduct(cutRight[npl/2] - cutRight[npl/2 -1], v0) <= 0)
                        cutRight[npl/2 -1] = cutRight[npl/2 -2];
                    if (CVector3d::dotProduct(cutRight[npl/2 +1] - cutRight[npl/2], v0) <= 0)
                        cutRight[npl/2 +1] = cutRight[npl/2 +2];
                }
                else {   // compute other points
                    cutRight[i] = right[i] + rotateNormalized(-M_PI/2, v4) * hirW;
                }
            }
        }
        else { // upper right side is a point but not lower side
            v4 = right[npl-1] - right[npl-2];

            for (i = npl/2 +1 ; i < npl ; i++)
                cutRight[i] = right[i] + rotateNormalized(-M_PI/2, v4) * hirW;
        }
    }
    else if ( v4.length() >= minSize ) {   // only lower right side is a point
        v3 = v4;
        //v3 = right[npl-2] - right[npl/2];
        /*        if (v3.length() == 0)
                {
                        cout << "AddHems v3=0 : about to crash 13" << endl;
                    for (i = 0 ; i < npl ; i++)
                        cout << "pt " << i << " xyz= " << right[i] << endl;
                }
        */
        for (i = 0 ; i < npl/2  ; i++)
            cutRight[i] = right[i] + rotateNormalized(-M_PI/2, v4) * lorW;

        for (i = npl/2 +1 ; i < npl ; i++)
            cutRight[i] = right[i] + rotateNormalized(-M_PI/2, v4) * hirW;

        v4 = right[npl-1] - right[npl-2];
        //    if (v4.length() == 0) cout << "AddHems v4=0 about to crash 13" << endl;
    }
    else {  // complete right side is a point
        if ( botW == 0 )
            v = v6;
        else
            v = ( v6 + v8 );

        if (v.length() == 0) cout << "CPanel::add6Hems v=v6=0 or v=v6+v8=0 about to crash 14" << endl;

        for (i = 0 ; i< npl ; i++)
            cutRight[i] = right[i] + v.normalized() * lorW;

        v3 = rotateNormalized(M_PI/2, v);
        v4 = v3;
    }

    ///* Rejoining the 4 corners of the cut panel */
    // cout << "Rejoining 4 corners" << endl;
    /// lower left
    if (v5.length() == 0)
        cout << "CPanel::add6Hems v5=0 about to crash 15" << endl;
    Line1 = CSubSpace::line( cutBottom[0] , v5 );

    if (v6.length() == 0)
        cout << "CPanel::add6Hems v6=0 about to crash 16" << endl;
    Line2 = CSubSpace::line( cutLeft[0] , v1 );

    pt = Line1.intersectionPoint(Line2, "lower left corner");

    /* Adjust the lower left point [0] to be at intersection */
    cutBottom[0] = pt;
    cutLeft[0] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the point pt
     */
    for (i = 0 ; i < npl/2 ; i++) {
        if (CVector3d::dotProduct(cutLeft[i] - pt, v1) <= 0)
            cutLeft[i] = pt;

        if (CVector3d::dotProduct(cutBottom[i] - pt, v5) <= 0)
            cutBottom[i] = pt;
    }

    /// lower right
    Line1 = CSubSpace::line( cutBottom[npb-1] , v6 );
    Line2 = CSubSpace::line( cutRight[0] , v3 );

    pt = Line1.intersectionPoint(Line2, "lower right corner");

    /* Adjust the lower left point [0] to be at intersection */
    cutBottom[npb-1] = pt;
    cutRight[0] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the point pt
     */
    for (i = 1 ; i < npl/2 ; i++) {
        if (CVector3d::dotProduct(cutRight[i] - pt, v3) <= 0)
            cutRight[i] = pt;
        if (CVector3d::dotProduct(cutBottom[npb-1-i] - pt, v6) >= 0)
            cutBottom[npb-1-i] = pt;
    }

    /// upper left
    Line1 = CSubSpace::line( cutTop[0] , v7 );
    Line2 = CSubSpace::line( cutLeft[npl-1] , v2 );

    pt = Line1.intersectionPoint(Line2, "upper left corner");

    /* Adjust the upper left point to be at intersection */
    cutTop[0] = pt;
    cutLeft[npl-1] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the intersect point pt
     */
    for (i = 0 ; i < npl/2 ; i++) {
        if (CVector3d::dotProduct(cutLeft[npl -1 -i] - pt, v2) >= 0)
            cutLeft[npl-1-i] = pt;
        if (CVector3d::dotProduct(cutTop[i] - pt, v7) <= 0)
            cutTop[i] = pt;
    }

    /// upper right
    Line1 = CSubSpace::line( cutTop[npb-1] , v8 );
    Line2 = CSubSpace::line( cutRight[npl-1] , v4 );

    pt = Line1.intersectionPoint(Line2, "upper right corner");

    /* Adjust the upper right point to be at intersection */
    cutTop[npb-1] = pt;
    cutRight[npl-1] = pt;

    /* Scan the first few points of the cut edges to make sure
     * that they are not on the wrong side of the intersect point pt
     */
    // cout << "CPanel::add6Hems scan for overlap " << endl;
    for (i = 1 ; i < npl/2 ; i++) {
        if (CVector3d::dotProduct(cutRight[npl-1-i] - pt, v4) >= 0)
            cutRight[npl-1-i] = pt;

        if (CVector3d::dotProduct(cutTop[npb-1-i] - pt, v6) >= 0)
            cutTop[npb-1-i] = pt;
    }
    // cout << "End CPanel::add6Hems" << endl << endl;
} /// end add6Hems ////////////////////////////////////////


/** Rotates a panel around a point.
 *
 *  p = centre of rotation
 *  angle = angle in radians
 *  axis = the axis around which to rotate
 */
CPanel CPanel::rotated(const CPoint3d &p, real angle, Qt::Axis axis) const
{
    CVector3d v;
    v[axis] = 1;

    CMatrix4x4 matrix;
    matrix.translate(p);
    matrix.rotate(angle, v);
    matrix.translate(-p);

    return transformed(matrix);
}


/** Transform a panel.
 */
CPanel CPanel::transformed(const CMatrix4x4 &m) const
{
    CPanel panel;
    panel.hasHems = hasHems;

    panel.label = label.transformed(m);

    panel.left = left.transformed(m);
    panel.right = right.transformed(m);
    panel.top = top.transformed(m);
    panel.bottom = bottom.transformed(m);

    panel.cutLeft = cutLeft.transformed(m);
    panel.cutRight = cutRight.transformed(m);
    panel.cutTop = cutTop.transformed(m);
    panel.cutBottom = cutBottom.transformed(m);

    return panel;
}


// operators

/** Performs a 3D translation of the panel by a given vector.
 */
CPanel CPanel::operator+ (const CVector3d &transl) const
{
    CMatrix4x4 matrix;
    matrix.translate(transl);
    return transformed(matrix);
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
    resize(nbpoints);
}


/** Makes the side a straight line between two points.
 */
void CSide::fill( const CPoint3d &p1 , const CPoint3d &p2 )
{
    switch ( size() )
    {
    case 0:
        return;
    case 1:
        at(0) = p1;
        return;
    }

    for (unsigned int i = 0 ; i < size() ; i++)
        at(i) = p1 + ( p2 - p1 ) * ( real(i) / (size() -1) );
}


/** Makes the side a two-segment line between three points.
 */
void CSide::fill( const CPoint3d &p1 , const CPoint3d &p2 , const CPoint3d &p3 )
{
    switch ( size() )
    {
    case 0:
        return;
    case 1:
        at(0) = p1;
        return;
    case 2:
        at(0) = p1;
        at(1) = p3;
        return;
    }

    unsigned int n1  = int( size() ) / 2;
    for (unsigned int i = 0 ; i < size() ; i++)
    {
        if ( i <= n1 )
            at(i) = p1 + (p2 - p1) * (real(i) / n1);
        else
            at(i) = p2 + (p3 - p2) * (real(i - n1) / (size() -n1 -1) );
    }
}


/** Transform a CSide.
 */
CSide CSide::transformed(const CMatrix4x4 &m) const
{
    CSide s( size() );

    for (unsigned int i = 0 ; i < size() ; i++)
        s[i] = m * at(i);

    return s;
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
    o << "== CPanelLabel : name ==" << endl << lb.name << endl;
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
    for (unsigned int i = 0 ; i < s.size() ; i++)
    {
        out << "#" << i << "\t" << s[i] << endl;
    }
    return out;
}

