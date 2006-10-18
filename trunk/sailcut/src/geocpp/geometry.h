/*
 * Copyright (C) 2002-2006 Jeremy Laine
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

#include "vector.h"

class CMatrix;
class CSubSpace;


/** Generic two dimensional real-valued vector.
 */
class CVector2d : public CVector
{
public:
    // construction & destruction
    CVector2d(const CVector &v);
    CVector2d(const CMatrix &m);
    /** Constructor. */
    CVector2d(const real &x = 0, const real &y = 0)
    {
        m_dim = 2;
        m_data = new real[2];
        m_data[0] = x;
        m_data[1] = y;
    }

    /** Returns the first coordinate. */
    real &x(void)
    {
        return m_data[0];
    }
    /** Returns the second coordinate. */
    real &y(void)
    {
        return m_data[1];
    }

    /** Assignment operator. */
    CVector2d &operator=(const CVector2d &v)
    {
        memcpy(m_data,v.m_data,sizeof(real)*2);
        return *this;
    }

};


/** Generic three dimensional real-valued vector.
 */
class CVector3d : public CVector
{
public:
    // construction & destruction
    CVector3d(const CVector& v);

    /** Column matrix to vector conversion. */
    CVector3d(const CMatrix& m);
    /** Constructor .*/
    CVector3d(const real &x = 0, const real &y = 0, const real &z = 0)
    {
        m_dim = 3;
        m_data = new real[3];
        m_data[0] = x;
        m_data[1] = y;
        m_data[2] = z;
    }

    // access to coords
    /** Returns the first coordinate. */
    real &x(void) const
    {
        return m_data[0];
    }
    /** Returns the second coordinate. */
    real &y(void) const
    {
        return m_data[1];
    }
    /** Returns the third coordinate. */
    real &z(void) const
    {
        return m_data[2];
    }

    // 3D only!
    /** Cross product, produces a vector orthogonal
     *  to the two original vectors. */
    CVector3d cross(const CVector3d &) const;
    /** Assignment. */
    CVector3d &operator=(const CVector3d &v)
    {
        memcpy(m_data,v.m_data,sizeof(real)*3);
        return *this;
    }
};


/** 2d real-valued point
 */
typedef CVector2d CPoint2d;


/** 3d real-valued point
 */
typedef CVector3d CPoint3d;


/** 3D box.
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
