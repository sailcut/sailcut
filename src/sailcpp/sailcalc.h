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

#ifndef SAILCALC_H
#define SAILCALC_H

#include <geocpp/geocpp.h>

/** @defgroup SailCpp Sail computations library
 *
 * SailCpp is the library that performs all the plotting calculations.
 * It provides classes for generating sails, hulls, rigs and boats from
 * their definitions.
 */

/* computation of parabolic edge round */
real RoundP( const real &x, const int &p );
/* computation of angle of triangle defined by 3 sides */
real Atriangle( const real &a, const real &b, const real &c );
/* computation of angle of triangle defined by 3 points */
real Atriangle3d ( const CPoint3d &pta, const CPoint3d &ptb, const CPoint3d &ptc );
/* computation of distance of a point to a line defined by 2 points */
real Distance3d(const CPoint3d &pta, const CPoint3d &ptb, const CPoint3d &ptc);

/* logical viewport calculation */
CRect3d calcLRect(const CRect3d &viewRect, const CRect3d &objRect, const CPoint3d center, real zoom );

#endif
