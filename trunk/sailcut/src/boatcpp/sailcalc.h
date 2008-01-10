/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SAILCALC_H
#define SAILCALC_H

#include <geocpp/geocpp.h>

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
