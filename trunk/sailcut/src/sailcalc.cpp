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

#include "sailcalc.h"


/** Compute the normalised roach or round of a sail side
 *  Return the value of the normalised round at position X
 *  X is the relative position of the point along the straight edge
 *  P is the position of the maximum round in percent of the edge length
 *  The curve is a parabola on either side of the point P
 *
 * @author Robert Laine alias Sailcuter
 */
real RoundP( const real &x, const int &p )
{
    /*
      X (0..1) is the relative position of the point along the edge
      P is the integer position of the maximum round in percent of the edge length
      The curve is a parabola on either side of the point P 
      Return Y (0..1) is the value of the normalised round at position X
    */

    real p1 = .5, x1 = 0 , y = 0;

    p1 = real(p) / 100; 
    // limit the position of maximum round of side
    if (p1 < 0.01)
        p1=0.01;
    if (p1 > 0.99)
        p1=0.99;
    
    // limit the computation of the domain 0..1
    if ( x <= 0 )
        y = 0;
    else if ( x >= 1 )
        y = 0;
    else if ( x > p1 )
    {
        x1 = (x - p1) / (1 - p1);
        y  = 1 - x1 * x1;
    }
    else
    {
        x1 = 1 - x / p1;
        y  = 1 - (x1 * x1);
    }
    //
    return y;
}


/** Compute the angle of a 2D triangle from its 3 sides length
 *  Return the angle in radian opposite to side a of the triangle
 *  a b and c are the length of the sides of the triangle 
 * 
 * @author Robert Laine alias Sailcuter
 */
real Atriangle( const real &a, real const &b, const real &c )
{
    /*
      A  B and C are the length of the sides of the triangle
      The returned angle AA is the angle opposite to side A expressed in Radians
    */

    real per = 0, AA = 0;
    per = (a + b + c) / 2;

    if ( per <= EPS )
        AA = PI /3;
    else if ( fabs(per-a) <= EPS )
        AA = PI;
    else
        AA = 2 * atan (sqrt ((per-b) * (per-c) / (per * (per-a)) ) ) ;
    //
    return AA;
}


/** Compute the angle of a 3D triangle from its 3 corner points
 *  Return the angle in radian at point pta of the triangle
 *  Triangle is defined by the 3d points pta, ptb, ptc
 * 
 * @author Robert Laine alias Sailcuter
 */
real Atriangle3d ( const CPoint3d &pta, const CPoint3d &ptb, const CPoint3d &ptc )
{
    /*
      The triangle is defined by 3d points pta, ptb, ptc
      Return the angle AA at the corner pta of the triangle
      The returned angle AA is expressed in Radians
    */
    real AA=0, a=0, b=0, c=0, per=0;

    a = sqrt( (ptc.x()-ptb.x()) * (ptc.x()-ptb.x())
              +(ptc.y()-ptb.y()) * (ptc.y()-ptb.y())
              +(ptc.z()-ptb.z()) * (ptc.z()-ptb.z()) );
    b = sqrt( (pta.x()-ptc.x()) * (pta.x()-ptc.x())
              +(pta.y()-ptc.y()) * (pta.y()-ptc.y())
              +(pta.z()-ptc.z()) * (pta.z()-ptc.z()) );
    c = sqrt( (ptb.x()-pta.x()) * (ptb.x()-pta.x())
              +(ptb.y()-pta.y()) * (ptb.y()-pta.y())
              +(ptb.z()-pta.z()) * (ptb.z()-pta.z()) );
              
    per =(a + b + c)/2;

    if ( per <= EPS )
        AA = PI /3;
    else if ( fabs(per-a) <= EPS )
        AA = PI;
    else
        AA = 2 * atan (sqrt ((per-b) * (per-c) / (per * (per-a)) ) ) ;
    //
    return AA;
}


/** Compute the distance from a point pta
 *  to the line defined by the 2 points ptb and ptc
 *
 * @author Robert Laine alias Sailcuter
 */
real Distance3d(const CPoint3d &pta, const CPoint3d &ptb, const CPoint3d &ptc)
{
    /*  The two points ptb and ptc defining the baseline also define
        its positive direction from point ptb toward point ptc.
        
        It is assumed that the line ptb toward ptc and point pta define
        a plane not far from the X-Y plane.
        
        The routine then returns the distance d with a positive sign
        if the point pta is left of the line ptb=>ptc
        The sign of d is negative if pta is right of the line.
    */
    real d;
    CVector3d Va = CVector3d( pta - ptb );
    CVector3d Vb = CVector3d( ptc - ptb).unit();
    CVector3d Vd = Vb.cross(Va);
    d = Vd.norm();
    if ( Vd.z() < 0 )
        d = -d;
    //
    return d;
}
