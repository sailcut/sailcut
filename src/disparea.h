/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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

#ifndef DISPAREA_H
#define DISPAREA_H

#include <geocpp/geocpp.h>


/** A generic display area
 *
 * @see CSailDisp
 */
class CDispArea
{
public:
    CDispArea(real izoom = 0.8, const CPoint3d icenter = CPoint3d());

    CRect3d calcLRect(const CRect3d &viewRect, const CRect3d &objRect);

protected:
    /** The center of the viewing area, in logical coordinates */
    CPoint3d center;

    /** The zoom factor for the view */
    real zoom;

    /** Has the area been resized since last redraw */
    bool wasResized;
};


#endif
