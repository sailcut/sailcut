/*
 * Copyright (C) 2002-2008 Jeremy Laine
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

#ifndef GEOCPP_GEOMETRY_H
#define GEOCPP_GEOMETRY_H

#include <geocpp/vector.h>

class CMatrix;
class CSubSpace;


/** Generic two dimensional real-valued vector.
 */
class CVector2d : public CVector
{
public:
    /** Constructor. */
    CVector2d(const real &x = 0, const real &y = 0)
        : CVector(2)
    {
        (*this)[0] = x;
        (*this)[1] = y;
    }

    /** Copy constructor.
     */
    CVector2d(const CVector &v)
        : CVector(v)
    {
        resize(2);
    };

    /** Returns the first coordinate. */
    real &x(void)
    {
        return (*this)[0];
    }
    /** Returns the second coordinate. */
    real &y(void)
    {
        return (*this)[1];
    }

    /** Assignment operator. */
    CVector2d &operator=(const CVector2d &v)
    {
        (*this)[0] = v[0];
        (*this)[1] = v[1];
        return *this;
    }

};


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
    real &x(void)
    {
        return (*this)[0];
    }
    real x(void) const
    {
        return (*this)[0];
    }

    /** Returns the second coordinate. */
    real &y(void)
    {
        return (*this)[1];
    }
    real y(void) const
    {
        return (*this)[1];
    }

    /** Returns the third coordinate. */
    real &z(void)
    {
        return (*this)[2];
    }
    real z(void) const
    {
        return (*this)[2];
    }

    // 3D only!

    /** Cross product. Produces a vector orthogonal
    *  to the two original vectors.
    */
    CVector3d cross(const CVector3d &v) const
    {
        CVector3d ret;
        for (int i=0; i<3; i++)
            ret[i] = (*this)[(i+1)%3] * v[(i+2)%3] - (*this)[(i+2)%3] * v[(i+1)%3];
        return ret;
    }

    /** Assignment. */
    CVector3d &operator=(const CVector3d &v)
    {
        (*this)[0] = v[0];
        (*this)[1] = v[1];
        (*this)[2] = v[2];
        return *this;
    }
};


/** 2d real-valued point
 */
typedef CVector2d CPoint2d;


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


/** Helper class to create 2D lines.
 */
class CSubSpace2d
{
public:
    static CSubSpace line(const CPoint2d& p, const CVector2d& v);
};


/** Helper class to create 3D subspaces (lines and planes)
 */
class CSubSpace3d
{
public:
    static CSubSpace line(const CPoint3d& p, const CVector3d& v);
    static CSubSpace plane(const CPoint3d& p, const CVector3d& v1, const CVector3d& v2);
};


#endif
