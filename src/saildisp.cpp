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

#include "saildisp.h"

/*****************************************************************************
 
                          CSailDisp class
 
*****************************************************************************/

/** Constructs a generic view area for a sail.
 */
CSailDisp::CSailDisp()
{
    // initialise data
    m = CMatrix::id(3);
    //setSail(sail);
    drawLabels = false;
    zoom=0.8;
}


/** Rotates local copy of the sail by a given azimuth and elevation.
 */
void CSailDisp::calcSailDisp()
{
    sailDisp = sailBase.rotate( rectBase.center(), m);
}


/** Resets display zoom and center to their default values.
 */
void CSailDisp::resetZoomCenter()
{
    center=CPoint3d(0,0,0);
    zoom = 0.8;
    calcLRect(m_vRect, rectBase);
}


/** Converts screen coordinates to logical coordinates.
 */
CPoint3d CSailDisp::screenToLogical( const int x, const int y ) const
{
    // avoid division by zero
    if ((m_vRect.width()==0)||(m_vRect.height()==0))
        return center;

    return center + CVector3d( m_lRect.width() * ( real(x) / m_vRect.width() - 0.5 ),
                               m_lRect.height() * ( 0.5 - real(y) / m_vRect.height() ), 0);
}


/** Sets the azimuth and elevation view angles.
 *
 * @param azimuth azimuth in degrees
 * @param elevation elevation in degrees
 */
void CSailDisp::setAngle( real azimuth, real elevation )
{
    m = CMatrix::rot3d(0, PI/180*elevation) * CMatrix::rot3d(1, PI/180*azimuth);
    calcSailDisp();
}


/** Sets the center of the display.
 *
 * @param newCenter the new center
 */
void CSailDisp::setCenter( CPoint3d newCenter )
{
    center = newCenter;
    m_lRect = calcLRect(m_vRect, rectBase);
}


/** Sets the sail that is to be displayed.
 *  and center it.
 */
void CSailDisp::setSail( const CSail &sail )
{
    sailBase = sail + CVector3d( -sail.boundingRect().center() );
    rectBase = sailBase.boundingRect();
    calcSailDisp();
    m_lRect = calcLRect(m_vRect, rectBase);
}


/** Set the size of the viewing rectangle.
 */
void CSailDisp::setVRect(const int width, const int height)
{
    m_vRect.min=CPoint3d(0,0,0);
    m_vRect.max=CPoint3d(width,height);
    m_lRect = calcLRect(m_vRect, rectBase);
}


/** Sets the zoom factor
 *
 * @param newZoom
 */
void CSailDisp::setZoom(real newZoom)
{
    zoom = newZoom;
    m_lRect = calcLRect(m_vRect, rectBase);
}

/** Zooms IN by a factor 2.
 */
void CSailDisp::zoomIn()
{
    setZoom(zoom * 2);
}


/** Zooms OUT by a factor 2.
 */
void CSailDisp::zoomOut()
{
    setZoom(zoom / 2);
}
