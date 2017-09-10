/*
 * Copyright (C) 1993-2017 Robert & Jeremy Laine
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

#include <geocpp/vector.h>


/**
 * Returns true if v1 and v2 are equal, allowing for a small fuzziness factor
 * for floating-point comparisons; false otherwise.
 */
bool qFuzzyCompare(const CVector3d &v1, const CVector3d &v2)
{
    for (size_t i = 0; i < 3; i++)
        if (fabs(v1[i] - v2[i]) > EPS)
            return false;
    return true;
}


/** Returns the vector's length.
 */
real CVector3d::length() const
{
    return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}


/** Returns corresponding unit length vector for non-zero vectors
 * and zero vector otherwise.
 */
CVector3d CVector3d::normalized() const
{
    const real n = length();
    if (n<EPS)
        return CVector3d();
    else
        return *this*(1/n);
}


/** Cross product. Produces a vector orthogonal to the two original vectors.
 */
CVector3d CVector3d::crossProduct(const CVector3d &v1, const CVector3d &v2)
{
    CVector3d ret;
    for (int i = 0; i < 3; i++)
        ret[i] = v1[(i+1)%3] * v2[(i+2)%3] - v1[(i+2)%3] * v2[(i+1)%3];
    return ret;
}


/** Dot (real) product.
 */
real CVector3d::dotProduct(const CVector3d &v1, const CVector3d &v2)
{
    return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v1.m_z;
}


/** Outputs a CVector3d to a stream.
 */
std::ostream& operator<<(std::ostream &o, const CVector3d &v)
{
    o << v.x() << "\t" << v.y() << "\t" << v.z();
    return o;
}
