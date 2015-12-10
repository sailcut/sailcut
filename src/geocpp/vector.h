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
typedef float real;

const real EPS = 0.00001;

/** 3d real-valued vector.
 *
 * @ingroup GeoCpp
 */
typedef QVector3D CVector3d;


/** 3d real-valued point
 *
 * @ingroup GeoCpp
 */
typedef QVector3D CPoint3d;


ostream& operator<<(ostream &o, const CVector3d &v);

inline
real qDegreesToRadians(int degrees)
{
    return qDegreesToRadians(real(degrees));
};


#endif
