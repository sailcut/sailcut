/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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

#ifndef GEOCPP_MATRIX4X4_H
#define GEOCPP_MATRIX4X4_H

#include <geocpp/vector.h>


class CMatrix4x4
{
public:
    CMatrix4x4();

    void rotate(real angle, const CVector3d &vector);
    void translate(const CVector3d &vector);

private:
    real m[4][4];

    CMatrix4x4& operator*=(const CMatrix4x4 &other);
    friend CVector3d operator*(const CMatrix4x4 &matrix, const CVector3d &vector);
};

inline
CVector3d operator*(const CMatrix4x4 &matrix, const CVector3d &vector)
{
    real x, y, z, w;

    x = vector.x() * matrix.m[0][0] +
        vector.y() * matrix.m[1][0] +
        vector.z() * matrix.m[2][0] +
        matrix.m[3][0];
    y = vector.x() * matrix.m[0][1] +
        vector.y() * matrix.m[1][1] +
        vector.z() * matrix.m[2][1] +
        matrix.m[3][1];
    z = vector.x() * matrix.m[0][2] +
        vector.y() * matrix.m[1][2] +
        vector.z() * matrix.m[2][2] +
        matrix.m[3][2];
    w = vector.x() * matrix.m[0][3] +
        vector.y() * matrix.m[1][3] +
        vector.z() * matrix.m[2][3] +
        matrix.m[3][3];

    if (w == 1.0)
        return CVector3d(x, y, z);
    else
        return CVector3d(x / w, y / w, z / w);
}

#endif
