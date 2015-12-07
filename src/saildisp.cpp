/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
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
    drawLabels = false;
    zoom = 0.8;
}


/** Rotates local copy of the object by a given azimuth and elevation.
 */
void CSailDisp::calcDispObject()
{
    dispObject = baseObject.rotate( baseRect.center(), m );
}


/** Resets display zoom and center to their default values.
 */
void CSailDisp::resetZoomCenter()
{
    center = baseRect.center();
    zoom = 0.8;
    logicalRect = calcLRect(viewRect, baseRect, center, zoom);
}


/** Converts screen coordinates to logical coordinates.
 */
CPoint3d CSailDisp::screenToLogical( const int x, const int y ) const
{
    // avoid division by zero
    if ((viewRect.width()==0)||(viewRect.height()==0))
        return center;

    return center + CVector3d( logicalRect.width() * ( real(x) / viewRect.width() - 0.5 ),
                               logicalRect.height() * ( 0.5 - real(y) / viewRect.height() ), 0);
}


/** Sets the azimuth and elevation view angles.
 *
 * @param azimuth azimuth in degrees
 * @param elevation elevation in degrees
 */
void CSailDisp::setAngle( real azimuth, real elevation )
{
    m = CMatrix::rot3d(0, PI/180*elevation) * CMatrix::rot3d(1, PI/180*azimuth);
    calcDispObject();
}


/** Sets the center of the display.
 *
 * @param newCenter the new center
 */
void CSailDisp::setCenter( const CPoint3d &newCenter )
{
    center = newCenter;
    logicalRect = calcLRect(viewRect, baseRect, center, zoom);
}


/** Sets the object that is to be displayed
 *  and center the view.
 */
void CSailDisp::setObject( const CPanelGroup &obj )
{
    baseObject = obj;
    baseRect = baseObject.boundingRect();

    // handle case where the bounding rectangle is flat
    if (baseRect.height() == 0)
        baseRect.max.setY(baseRect.max.y() + 1);
    if (baseRect.width() == 0)
        baseRect.max.x() += 1;
    center = baseRect.center();

    calcDispObject();
    logicalRect = calcLRect(viewRect, baseRect, center, zoom);
}


/** Set the size of the viewing rectangle.
 */
void CSailDisp::setViewRect( const CRect3d &rect )
{
    viewRect = rect;
    logicalRect = calcLRect(viewRect, baseRect, center, zoom);
}


/** Sets the zoom factor
 *
 * @param newZoom
 */
void CSailDisp::setZoom(real newZoom)
{
    zoom = newZoom;
    logicalRect = calcLRect(viewRect, baseRect, center, zoom);
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
