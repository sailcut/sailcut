/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
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

#ifndef SAILDISP_H
#define SAILDISP_H

#include "sailcpp/panelgroup.h"
#include "sailcpp/sailcalc.h"


/** A generic display area for a panel group.
 *
 * @see CSailDispLabel
 * @see CSailDispGL
 */
class CSailDisp
{
public:
    CSailDisp();

    /** The destructor. */
    virtual ~CSailDisp()
    {}
    ;

    /** Acessor for the logical viewport rectangle */

    /** Redraw the view. */
    virtual void redraw() = 0;
    virtual void resetZoomCenter( void );
    virtual CPoint3d screenToLogical( const int x, const int y ) const;

    void setAzimuth(real azimuth);
    void setElevation(real elevation);

    CPoint3d center() const;
    void setCenter(const CPoint3d &newCenter);

    void setObject( const CPanelGroup &objs );
    void setViewRect( const CRect3d &rect );

    real zoom() const;
    void setZoom(real newZoom);

    void zoomIn();
    void zoomOut();

    /** Should labels be drawn ? */
    bool drawLabels;

protected:
    CPanelGroup dispObject() const;
    CRect3d logicalRect() const;

private:
    real m_azimuth;
    /** The center of the viewing area, in logical coordinates */
    CPoint3d m_center;
    real m_elevation;
    real m_zoom;

    /** Viewport rectangle, in device coordinates */
    CRect3d viewRect;
    /** Bounding rectangle of the unrotated object */
    CRect3d baseRect;
    /** Unrotated copy of the object */
    CPanelGroup baseObject;
};


#endif
