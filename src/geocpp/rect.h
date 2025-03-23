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

#ifndef GEOCPP_RECT_H
#define GEOCPP_RECT_H

#include <geocpp/vector.h>


/** Three dimensional box.
 *
 * @ingroup GeoCpp
 */
class CRect3d
{
public:
    /** Construct a new 3D box. */
    CRect3d()
    {}
    ;

    /** Construct a new 3D box given its corners.
     *
     * @param pmin
     * @param pmax
     */
    CRect3d(const CPoint3d &pmin, const CPoint3d &pmax)
        : min(pmin), max(pmax)
    {}
    ;

    // operators
    CRect3d operator+ (const CVector3d &transl) const;
    CRect3d operator* (const real r) const;

    /** Return the box's center. */
    CPoint3d center() const
    {
        return 0.5 * (min + max);
    };

    CRect3d expandToRatio(const real ratio) const;

    /** Return the box's width. */
    real width() const
    {
        return max.x()-min.x();
    };

    /** Return the box's height. */
    real height() const
    {
        return max.y()-min.y();
    };

    CRect3d join(const CRect3d& rect) const;

    /** lower-left corner */
    CPoint3d min;
    /** top-right corner */
    CPoint3d max;
};

#endif
