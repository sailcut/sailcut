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

#ifndef GEOCPP_SUBSPACE_H
#define GEOCPP_SUBSPACE_H

#include <geocpp/geometry.h>
#include <geocpp/matrix.h>

enum subspaceflags_t { GEOCPP_FROM_BASE, GEOCPP_FROM_EQS };

/** Class for describing subspaces of an euclidian vector space,
 *  for example the empty space, points, lines and planes.
 *
 * @ingroup GeoCpp
 */
class CSubSpace
{
public:
    CSubSpace();
    CSubSpace(const CVector3d &pi, const CMatrix &mi, subspaceflags_t createflags);
    CSubSpace(const CSubSpace &s);

    CSubSpace intersect(const CSubSpace &) const;
    CVector3d intersectionPoint(const CSubSpace &, const char *name) const;

    bool contains(const CVector3d &) const;
    int getdim() const;

    /** Accessor for the matrix. */
    const CMatrix& getm() const
    {
        return m;
    }

    /** Accessor for the point. */
    const CVector3d& getp() const
    {
        return p;
    }

private:
    bool isEmpty;
    CMatrix m;
    CVector3d p;
};

// global functions
ostream& operator<< (ostream &, const CMatrix &);
ostream& operator<< (ostream &, const CSubSpace &);

#endif
