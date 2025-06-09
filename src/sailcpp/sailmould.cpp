/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
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
#include <iostream>

#include "sailmould.h"

/**************************************************************************
                             CProfile class
***************************************************************************

                       construction / destruction

**************************************************************************/

/** The constructor.
 *
 * @param rDepth
 * @param rLeech
 * @param rLuff
 */
CProfile::CProfile( real rDepth, real rLeech, real rLuff)
        : depth(rDepth), kleech(rLeech), kluff(rLuff)
{
    calcMax();
}


/**************************************************************************

                            member functions

**************************************************************************/

/** Compute the depth of the profile at a point located at dX along the cord
 */
real CProfile::z( real dX ) const
{
    /*
       dX = absiss 0 to 1 along the cord of the profile
       kluff = luff shape factor
       kleech = leech shape factor
       depth = depth of the profile
       profile function is obtained by integrating twice the equation
         d2z/dx2 = kd * (-a*(1-X)^kluff - kleech * X)
       giving first the slope of the profile
         dz/dx = kd * (a*(1-X)^(kluff+1)/(kluff+1) - kleech/2 * X^2 + c)
       and finally the depth of the profile
         z = kd * (-a*(1-X)^(kluff+2)/((kluff+1)*(kluff+2)) - kleech/6 * X^3 +c * X + b)
         b and c are such that z=0 for X=0 AND X=1
         kd is such that the value of z at dX=xmax (max depth) is equal to depth
    */

    real a=1, b=0, c=0, kd=1, z=.1 ;
    a = 1 + kluff/4 ;// specific to family of profile
    b = a / ((kluff+2)*(kluff+1));
    c = kleech/6 - b;

    if (dX <= 0)  /* point is outside profile range */
        z = 0;
    else if (dX >= 1)  /* point is outside profile range */
        z = 0;
    else
    { /* point is inside profile range */
        /* compute depth normalisation coefficient */
        kd = depth / zmax;
        /* compute real depth z at point dX */
        z = kd * (-a*pow(1-dX,kluff+2)/((kluff+1)*(kluff+2)) - kleech/6 * pow(dX,3) + c * dX + b );
    }
    return z;
}

/** Compute the slope of the profile at a point located at dX along the cord
 */
real CProfile::slope( real dX ) const
{
    /*
       dX = absiss 0 to 1 along the cord of the profile
       kluff = luff shape factor
       kleech = leech shape factor
       depth = depth of the profile
       profile function is obtained by integrating twice the equation
         d2z/dx2 = kd * (-a*(1-X)^kluff - kleech * X)
       giving first the slope of the profile
         dz/dx = kd * (a*(1-X)^(kluff+1)/(kluff+1) - kleech/2 * X^2 + c)
       and finally the depth of the profile
         z = kd * (-a*(1-X)^(kluff+2)/((kluff+1)*(kluff+2)) - kleech/6 * X^3 +c * X + b)
         b and c are such that z=0 for X=0 AND X=1
         kd is such that the value of z at dX=xmax (max depth) is equal to depth
    */

    real a=1, b=0, c=0, kd=1, dz=.1 , x=0;
    a = 1 + kluff/4 ;// specific to family of profile
    b = a / ((kluff+2)*(kluff+1));
    c = kleech/6 - b;
    x = dX;

    if (x <= 0)  /* point is outside profile range */
        x = 0;
    else if (x >= 1)  /* point is outside profile range */
        x = 1;

    /* compute depth normalisation coefficient */
    kd = depth / zmax;
    /* compute real slope dz at point dX */
    dz = kd * ( a * pow(1-x,kluff+1) / (kluff+1) - kleech/2 * pow(x,2) + c );

    return dz;
}

/** Compute the camber of the profile at a point located at dX along the cord
 */
real CProfile::camber( real dX ) const
{
    /*
       dX = absiss 0 to 1 along the cord of the profile
       kluff = luff shape factor
       kleech = leech shape factor
       depth = depth of the profile
       profile function is obtained by integrating twice the equation
         d2z/dx2 = kd * (-a*(1-X)^kluff - kleech * X)
       giving first the slope of the profile
         dz/dx = kd * (a*(1-X)^(kluff+1)/(kluff+1) - kleech/2 * X^2 + c)
       and finally the depth of the profile
         z = kd * (-a*(1-X)^(kluff+2)/((kluff+1)*(kluff+2)) - kleech/6 * X^3 +c * X + b)
         b and c are such that z=0 for X=0 AND X=1
         kd is such that the value of z at dX=xmax (max depth) is equal to depth
    */

    real a=1, b=0, c=0, kd=1, dz=.1 , d2z=.1, camb=.1, x=0;
    a = 1 + kluff/4 ;// specific to family of profile
    b = a / ((kluff+2)*(kluff+1));
    c = kleech/6 - b;
    x = dX;

    if (x <= 0)  /* point is outside profile range */
        x = 0;
    else if (x >= 1)  /* point is outside profile range */
        x = 1;

    /* compute depth normalisation coefficient */
    kd = depth / zmax;
    /* compute real slope dz at point dX */
    dz = kd * ( a * pow(1-x,kluff+1) / (kluff+1) - kleech/2 * pow(x,2) + c );
    /* compute real d2z at point dX */
    d2z = kd * ( -a * pow(1-x,kluff) - kleech * x );
    /* compute real camber at point dX */
    camb = d2z / pow((1 + pow(dz,2)),1.5);

    return camb;
}

/** Compute the absiss x of the point of maximum depth of a profile
 */
void CProfile::calcMax()
{
    /* kluff = luff shape factor
       kleech = leech shape factor
       Profile function is defined by integrating twice the equation
         d2z/dx2 = kd * (-a*(1-x)^kluff - kleech * x)
         dz/dx = kd * (a*(1-x)^(kluff+1)/(kluff+1) - kleech/2 * x^2 + c)
         z = kd * (-a*(1-x)^(kluff+2)/((kluff+1)*(kluff+2)) - kleech/6 * x^3 +c * x + b)
         b and c are such that z=0 for x=0 AND x=1
    */

    real a=1, b=0, c=0;
    a = 1 + kluff/4;
    b = a / ((kluff+2)*(kluff+1));
    c = kleech/6 - b;

    /* scan from 16% point until slope become <= 0 */
    real x=.16, dz=.1;
    int n=16, step=8;
    while ((dz>0) && (n<60))
    {
        x = real(n)/100;
        dz = a*pow( 1-x, kluff+1)/(kluff+1) - kleech/2 * (x * x) + c;
        while ((dz<=0) && (step>1))
        {
            /* too big a step, go back halfway*/
            /* switch to fine stepping */
            step = step/2;
            if (step<2)
            {
                step = 2;
            }
            n = n - step;
            x = real(n)/100;
            dz = a*pow( 1-x, kluff+1)/(kluff+1) - kleech/2 * (x * x) + c;
            if (dz>0)
            {
                step = 1;
            }
        }
        n = n + step;
    }

    // we store xmax and zmax
    xmax = x;
    zmax = -a*pow(1-xmax,kluff+2)/ ((kluff+1)*(kluff+2)) - kleech/6 * pow(xmax,3) + c * xmax +b;
}


/**************************************************************************
                             CSailMould class
***************************************************************************

                       construction / destruction

**************************************************************************/

/**  Set the default vertical position of max depth
 *  and the 3 profiles factors [depth, kleech, kluff]
 */
CSailMould::CSailMould()
{
    vertpos = 40;
    profile.resize(3);
    profile[0] = CProfile( 0.02, 0.00, 0 );
    profile[1] = CProfile( 0.08, 0.04, 3 );
    profile[2] = CProfile( 0.06, 0.03, 5 );
}


/**************************************************************************

                            member functions

**************************************************************************/

/** Interpolate the depth and coefficients of the profile at height h
 */
CProfile CSailMould::interpol ( const real h ) const
{
    if ( profile.size() < 3)
        std::cout << "profile < 3 !!" << std::endl;

    CProfile p;
    real pv = real(vertpos) / 100;
    real dpth = 0;
    real hr;

    if ( h <= 0 )   // at or below lower profile
    {
        /*p = profile[0];*/
        hr = h / pv;
        dpth = profile[0].getDepth() + (profile[1].getDepth()-profile[0].getDepth()) * hr;
        if ( dpth < 0 ) dpth = 0;
        p = CProfile( dpth, profile[0].getLeech() , profile[0].getLuff() );
    }
    else if ( h < pv )   // below max depth
    {
        hr = h / pv;
        dpth = profile[0].getDepth() + (profile[1].getDepth()-profile[0].getDepth()) * (1-(1-hr)*(1-hr));
        if ( dpth < 0 ) dpth = 0;
        p = CProfile( dpth ,
                      profile[0].getLeech() + (profile[1].getLeech()-profile[0].getLeech()) * hr,
                      profile[0].getLuff()  + (profile[1].getLuff()-profile[0].getLuff()) * hr );
    }
    else if ( h < 1 )   // above max depth and below peak
    {
        hr = (h-pv) / (1-pv);
        dpth = profile[1].getDepth() + (profile[2].getDepth()-profile[1].getDepth()) * hr*hr;
        if ( dpth < 0 ) dpth = 0;
        p = CProfile( dpth ,
                      profile[1].getLeech() + (profile[2].getLeech()-profile[1].getLeech()) * hr,
                      profile[1].getLuff()  + (profile[2].getLuff()-profile[1].getLuff()) * hr );
    }
    else  // peak and above
    {
        p = profile[2];
    }

    return p;
}
