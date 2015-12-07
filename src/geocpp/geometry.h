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

#ifndef GEOCPP_GEOMETRY_H
#define GEOCPP_GEOMETRY_H

#include <geocpp/vector.h>

class CMatrix;
class CSubSpace;


/** Generic three dimensional real-valued vector.
 *
 * @ingroup GeoCpp
 */
class CVector3d : public CVector
{
public:
    /** Constructor. */
    CVector3d(const real &x = 0, const real &y = 0, const real &z = 0)
        : CVector(3)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    /** Copy constructor.
    */
    CVector3d(const CVector &v)
        : CVector(v)
    {
        resize(3);
    };

    // access to coords
    /** Returns the first coordinate. */
    real x(void) const
    {
        return (*this)[0];
    }

    /** Sets the first coordinate. */
    void setX(real x)
    {
        (*this)[0] = x;
    }

    /** Returns the second coordinate. */
    real y(void) const
    {
        return (*this)[1];
    }

    /** Sets the second coordinate. */
    void setY(real y)
    {
        (*this)[1] = y;
    }

    /** Returns the third coordinate. */
    real z(void) const
    {
        return (*this)[2];
    }

    /** Sets the third coordinate. */
    void setZ(real z)
    {
        (*this)[2] = z;
    }

    static CVector3d crossProduct(const CVector3d &v1, const CVector3d &v2);
    static real dotProduct(const CVector3d &v1, const CVector3d &v2);

    /** Assignment. */
    CVector3d &operator=(const CVector3d &v)
    {
        (*this)[0] = v[0];
        (*this)[1] = v[1];
        (*this)[2] = v[2];
        return *this;
    }
};


/** 3d real-valued point
 *
 * @ingroup GeoCpp
 */
typedef CVector3d CPoint3d;


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


/** Helper class to create 3D subspaces (lines and planes)
 */
class CSubSpace3d
{
public:
    static CSubSpace line(const CPoint3d& p, const CVector3d& v);
    static CSubSpace plane(const CPoint3d& p, const CVector3d& v1, const CVector3d& v2);
};


/** Outputs a CVector3d to a stream.
 */
inline
ostream& operator<<(ostream &o, const CVector3d &v)
{
    for (int i = 0; i < 3; i++)
        o << (i > 0 ? "\t" : "") << v[i];
    return o;
}

#endif
