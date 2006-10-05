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

#include "disparea.h"

/** Constructs a generic view area.
 */
CDispArea::CDispArea(real izoom) :
        zoom(izoom), wasResized(1)
{}


/** Calculates logical viewport rectangle to match
 * the ratio of the device viewport
 */
CRect3d CDispArea::calcLRect(const CRect3d& viewRect, const CRect3d& objRect)
{
    CRect3d lRect;

    // avoid division by zero errors
    if ((viewRect.height() == 0) || (viewRect.width() == 0))
    {
        lRect.min = center;
        lRect.max = center;
        return lRect;
    }

    real viewAspect = viewRect.width() / viewRect.height();
    real objAspect = objRect.width() / objRect.height();

    // the viewing area may not match the proportions of the sail
    // so we set the logical viewport accordingly
    if (objAspect > viewAspect)
    {
        // we are limited by the width of the window, grow logical viewport's height
        lRect.min = CVector3d(objRect.min.x(),objRect.min.y()*(objAspect/viewAspect),0);
        lRect.max = CVector3d(objRect.max.x(),objRect.max.y()*(objAspect/viewAspect),0);
    }
    else
    {
        // we are limited by the height of the window, grow logical viewport's width
        lRect.min = CVector3d(objRect.min.x()*(viewAspect/objAspect),objRect.min.y(),0);
        lRect.max = CVector3d(objRect.max.x()*(viewAspect/objAspect),objRect.max.y(),0);
    }

    /*
        cout << "-------" << endl;
        cout << "objRect w: " << objRect.width() << ", h: "  << objRect.height() << endl;
        cout << "viewRect w: " << viewRect.width() << ", h: " << viewRect.height() << endl;
        cout << "lRect w: "<< lRect.width() << ", h: " << lRect.height() << endl;
        cout << "zoom : " << zoom << endl;
    */
    lRect = lRect*(1/zoom)+center;

    /*
        cout << "lRect w: " << lRect.width() << ", h: " << lRect.height() << endl;
        cout << "-------" << endl;
    */
    return lRect;
}

