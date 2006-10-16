/*
 * Copyright (C) 2002-2004 Jeremy Lain�
 * See AUTHORS file for a full list of contributors.
 * 
 * $Id: vect_geom.cpp,v 1.16 2006/10/03 11:45:17 jeremy_laine Exp $
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

#include "vect_geom.h"
#include "matrix.h"
#include "subspace.h"

/***************************************
 
            Generic 2D real vector
 
***************************************/

/** Copy constructor.
 */
CVector2d::CVector2d(const CVector &v)
        : CVector(2)
{
    for (unsigned int i = 0; i < 2; i++)
        if (i < v.m_dim)
            m_data[i] = v.m_data[i];
}



/** Column matrix to vector conversion.
 */
CVector2d::CVector2d(const CMatrix &m)
        : CVector(m.col(0))
{}


/***************************************
 
           Generic 3D real vector
 
***************************************/

/** Copy constructor.
 */
CVector3d::CVector3d(const CVector &v)
        : CVector(3)
{
    for (unsigned int i = 0; i < 3; i++)
        if (i < v.m_dim)
            m_data[i] = v.m_data[i];
}


/** Column matrix to vector conversion.
 */
CVector3d::CVector3d(const CMatrix &m)
        : CVector(m.col(0))
{}


/** Cross product.
 */
CVector3d CVector3d::cross(const CVector3d &v) const
{
    CVector3d ret;
    for(int i=0; i<3; i++)
        ret.m_data[i] = m_data[(i+1)%3]*v.m_data[(i+2)%3] - m_data[(i+2)%3]*v.m_data[(i+1)%3];
    return ret;
}


/***************************************
 
                 3D box
 
***************************************/

/** Translate box by a vector.
 *
 * @param transl translation vector
 */
CRect3d CRect3d::operator+ (const CVector3d &transl) const
{
    CRect3d ret;
    ret.min = min + transl;
    ret.max = max + transl;
    return ret;
}


/** Expand/shrink box by a coefficient.
 *
 * @param r coefficient
 */
CRect3d CRect3d::operator* (const real r) const
{
    CRect3d ret;
    ret.min = min*r;
    ret.max = max*r;
    return ret;
}


/***************************************
 
                 2D/3D subspaces
 
***************************************/

/** Create a CSubSpace representing a 2D line from a point and a vector.
 *
 * @param p a point of the line
 * @param v a vector along the line
 */
CSubSpace CSubSpace2d::line(const CPoint2d &p, const CVector2d &v)
{
    if (!v.norm())
        throw CException("CSubSpace2d::line : vector cannot be zero");

    return CSubSpace(p, v.matrix(), GEOCPP_FROM_BASE);
}


/** Create a CSubSpace representing a 3D line from a point and a vector.
 *
 * @param p a point of the line
 * @param v a vector along the line
 */
CSubSpace CSubSpace3d::line(const CPoint3d &p, const CVector3d &v)
{
    if (!v.norm())
        throw CException("CSubSpace3d::line : vector cannot be zero");

    return CSubSpace(p, v.matrix(), GEOCPP_FROM_BASE);
}


/** Create a CSubSpace representing a 3D plane from a point and two vectors.
 *
 * @param p a point of the plane
 * @param v1 a vector in the plane
 * @param v2 a second vector in the plane, not colinear with v1
 */
CSubSpace CSubSpace3d::plane(const CPoint3d &p, const CVector3d &v1, const CVector3d& v2)
{
    if (!v1.cross(v2).norm())
        throw CException("CSubSpace3d::plane : vectors cannot be colinear");

    CMatrix m(3,2);
    for (int i = 0; i < 3; i++)
    {
        m(i, 0) = v1[i];
        m(i, 1) = v2[i];
    }
    return CSubSpace(p, m, GEOCPP_FROM_BASE);
}

