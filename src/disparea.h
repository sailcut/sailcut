/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: disparea.h,v 1.7 2006/01/28 12:32:54 jeremy_laine Exp $
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

#ifndef DISPAREA_H
#define DISPAREA_H

#include "geocpp/vect_geom.h"


/** A generic display area
 *
 * @see CSailDisp
 */
class CDispArea
{
public:
    CDispArea(real izoom = 0.8);

    CRect3d calcLRect(const CRect3d &viewRect, const CRect3d &objRect);
    /** Get the current zoom factor. */
    real getZoom()
    {
        return zoom;
    }



protected:
    /** The center of the viewing area, in logical coordinates */
    CPoint3d center;

    /** The zoom factor for the view */
    real zoom;

    /** Has the area been resized since last redraw */
    bool wasResized;
};


#endif
