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

#ifndef GEOCPP_VECTOR_H
#define GEOCPP_VECTOR_H

#include <cmath>
#include <iostream>
#include <vector>

#include <QtMath>
#include <QMatrix4x4>
#include <QVector3D>


using namespace std;


// types and constants
typedef double real;

const real EPS = 1E-14;

/** 3d real-valued vector.
 *
 * @ingroup GeoCpp
 */
class CVector3d
{
public:
    /** Constructor. */
    CVector3d(const real &x = 0, const real &y = 0, const real &z = 0)
        : m_x(x)
        , m_y(y)
        , m_z(z)
    {
    };

    // access to coords
    /** Returns the first coordinate. */
    real x() const
    {
        return m_x;
    };

    /** Sets the first coordinate. */
    void setX(real x)
    {
        m_x = x;
    };

    /** Returns the second coordinate. */
    real y(void) const
    {
        return m_y;
    };

    /** Sets the second coordinate. */
    void setY(real y)
    {
        m_y = y;
    };

    /** Returns the third coordinate. */
    real z(void) const
    {
        return m_z;
    };

    /** Sets the third coordinate. */
    void setZ(real z)
    {
        m_z = z;
    };

    real length() const;
    CVector3d normalized() const;

    static CVector3d crossProduct(const CVector3d &v1, const CVector3d &v2);
    static real dotProduct(const CVector3d &v1, const CVector3d &v2);

    real &operator[](size_t idx)
    {
        return *(&m_x + idx);
    };

    real operator[](size_t idx) const
    {
        return *(&m_x + idx);
    };

    bool operator==(const CVector3d &v2) const
    {
        return m_x == v2.m_x && m_y == v2.m_y && m_z == v2.m_z;
    }

    /** Binary '+' operator (addition)
    */
    CVector3d operator+(const CVector3d& v2) const
    {
        return CVector3d(
            m_x + v2.m_x,
            m_y + v2.m_y,
            m_z + v2.m_z);
    };

    /** Unary '-' operator (return opposite)
     */
    CVector3d operator-() const
    {
        return CVector3d(-m_x, -m_y, -m_z);
    };

    /** Binary '-' operator (return difference)
    */
    CVector3d operator-(const CVector3d &v2) const
    {
        return CVector3d(
            m_x - v2.m_x,
            m_y - v2.m_y,
            m_z - v2.m_z);
    };

    /** Binary '* (multiply a vector by a real)
    */
    CVector3d operator*(const real& lambda) const
    {
        return CVector3d(
            m_x * lambda,
            m_y * lambda,
            m_z * lambda);
    };

private:
    real m_x;
    real m_y;
    real m_z;
};

/** 3d real-valued point
 *
 * @ingroup GeoCpp
 */
typedef CVector3d CPoint3d;

real qDegreesToRadians(int degrees);
bool qFuzzyCompare(const CVector3d &v1, const CVector3d &v2);

CVector3d operator*(const QMatrix4x4 &m, const CVector3d &v);

/** Binary '*' (multiply a scalar by a vector)
 */
inline
CVector3d operator*(const real& lambda, const CVector3d &v)
{
    return v * lambda;
}

ostream& operator<<(ostream &o, const CVector3d &v);

#endif
