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

#include <cmath>

#include <geocpp/matrix4x4.h>


CMatrix4x4::CMatrix4x4()
{
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void CMatrix4x4::rotate(real angle, const CVector3d &vector)
{
    if (angle == 0.0)
        return;

    const real c = std::cos(angle);
    const real s = std::sin(angle);
    const real ic = 1.0 - c;

    const CVector3d n = vector.normalized();
    const real x = n.x();
    const real y = n.y();
    const real z = n.z();

    CMatrix4x4 rot;
    rot.m[0][0] = x * x * ic + c;
    rot.m[1][0] = x * y * ic - z * s;
    rot.m[2][0] = x * z * ic + y * s;
    rot.m[3][0] = 0.0f;
    rot.m[0][1] = y * x * ic + z * s;
    rot.m[1][1] = y * y * ic + c;
    rot.m[2][1] = y * z * ic - x * s;
    rot.m[3][1] = 0.0f;
    rot.m[0][2] = x * z * ic - y * s;
    rot.m[1][2] = y * z * ic + x * s;
    rot.m[2][2] = z * z * ic + c;
    rot.m[3][2] = 0.0f;
    rot.m[0][3] = 0.0f;
    rot.m[1][3] = 0.0f;
    rot.m[2][3] = 0.0f;
    rot.m[3][3] = 1.0f;

    *this *= rot;
}

void CMatrix4x4::translate(const CVector3d& vector)
{
    const real vx = vector.x();
    const real vy = vector.y();
    const real vz = vector.z();

    m[3][0] += m[0][0] * vx + m[1][0] * vy + m[2][0] * vz;
    m[3][1] += m[0][1] * vx + m[1][1] * vy + m[2][1] * vz;
    m[3][2] += m[0][2] * vx + m[1][2] * vy + m[2][2] * vz;
    m[3][3] += m[0][3] * vx + m[1][3] * vy + m[2][3] * vz;
}

CMatrix4x4& CMatrix4x4::operator*=(const CMatrix4x4& other)
{
    float m0, m1, m2;
    m0 = m[0][0] * other.m[0][0]
            + m[1][0] * other.m[0][1]
            + m[2][0] * other.m[0][2]
            + m[3][0] * other.m[0][3];
    m1 = m[0][0] * other.m[1][0]
            + m[1][0] * other.m[1][1]
            + m[2][0] * other.m[1][2]
            + m[3][0] * other.m[1][3];
    m2 = m[0][0] * other.m[2][0]
            + m[1][0] * other.m[2][1]
            + m[2][0] * other.m[2][2]
            + m[3][0] * other.m[2][3];
    m[3][0] = m[0][0] * other.m[3][0]
            + m[1][0] * other.m[3][1]
            + m[2][0] * other.m[3][2]
            + m[3][0] * other.m[3][3];
    m[0][0] = m0;
    m[1][0] = m1;
    m[2][0] = m2;

    m0 = m[0][1] * other.m[0][0]
            + m[1][1] * other.m[0][1]
            + m[2][1] * other.m[0][2]
            + m[3][1] * other.m[0][3];
    m1 = m[0][1] * other.m[1][0]
            + m[1][1] * other.m[1][1]
            + m[2][1] * other.m[1][2]
            + m[3][1] * other.m[1][3];
    m2 = m[0][1] * other.m[2][0]
            + m[1][1] * other.m[2][1]
            + m[2][1] * other.m[2][2]
            + m[3][1] * other.m[2][3];
    m[3][1] = m[0][1] * other.m[3][0]
            + m[1][1] * other.m[3][1]
            + m[2][1] * other.m[3][2]
            + m[3][1] * other.m[3][3];
    m[0][1] = m0;
    m[1][1] = m1;
    m[2][1] = m2;

    m0 = m[0][2] * other.m[0][0]
            + m[1][2] * other.m[0][1]
            + m[2][2] * other.m[0][2]
            + m[3][2] * other.m[0][3];
    m1 = m[0][2] * other.m[1][0]
            + m[1][2] * other.m[1][1]
            + m[2][2] * other.m[1][2]
            + m[3][2] * other.m[1][3];
    m2 = m[0][2] * other.m[2][0]
            + m[1][2] * other.m[2][1]
            + m[2][2] * other.m[2][2]
            + m[3][2] * other.m[2][3];
    m[3][2] = m[0][2] * other.m[3][0]
            + m[1][2] * other.m[3][1]
            + m[2][2] * other.m[3][2]
            + m[3][2] * other.m[3][3];
    m[0][2] = m0;
    m[1][2] = m1;
    m[2][2] = m2;

    m0 = m[0][3] * other.m[0][0]
            + m[1][3] * other.m[0][1]
            + m[2][3] * other.m[0][2]
            + m[3][3] * other.m[0][3];
    m1 = m[0][3] * other.m[1][0]
            + m[1][3] * other.m[1][1]
            + m[2][3] * other.m[1][2]
            + m[3][3] * other.m[1][3];
    m2 = m[0][3] * other.m[2][0]
            + m[1][3] * other.m[2][1]
            + m[2][3] * other.m[2][2]
            + m[3][3] * other.m[2][3];
    m[3][3] = m[0][3] * other.m[3][0]
            + m[1][3] * other.m[3][1]
            + m[2][3] * other.m[3][2]
            + m[3][3] * other.m[3][3];
    m[0][3] = m0;
    m[1][3] = m1;
    m[2][3] = m2;
    return *this;
}
