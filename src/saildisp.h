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

#ifndef SAILDISP_H
#define SAILDISP_H

#include "panelgroup.h"
#include "geocpp/matrix.h"
#include "disparea.h"


/** A generic display area for a sail.
 *
 * @see CSailDispLabel
 * @see CSailDispGL
 */
class CSailDisp : public CDispArea
{
public:
    CSailDisp();

    /** The destructor. */
    virtual ~CSailDisp()
    {}
    ;

    /** Acessor for the logical viewport rectangle */
    virtual const CRect3d& getLogicalRect() const
    {
        return logicalRect;
    };

    /** Redraw the view. */
    virtual void redraw()
    {}
    ;
    virtual void resetZoomCenter( void );
    virtual CPoint3d screenToLogical( const int x, const int y ) const;
    virtual void setAngle( real azimuth, real elevation );
    virtual void setCenter( CPoint3d newCenter );
    virtual void setObjects( const vector<CPanelGroup> &objs );
    virtual void setViewRect( const CRect3d &rect );
    virtual void setZoom(real newZoom);
    virtual void zoomIn();
    virtual void zoomOut();

    /** Should labels be drawn ? */
    bool drawLabels;

private:
    void calcDispObjects();

protected:
    /** Rotated copy of the objects */
    vector<CPanelGroup> dispObjects;

private:    
    /** Rotation matrix for the view */
    CMatrix m;
    /** Viewport rectangle, in logical coordinates */
    CRect3d logicalRect;
    /** Viewport rectangle, in device coordinates */
    CRect3d viewRect;
    /** Bounding rectangle of the unrotated objects */
    CRect3d baseRect;
    /** Unrotated copy of the objectsl */
    vector<CPanelGroup> baseObjects;
};


#endif
