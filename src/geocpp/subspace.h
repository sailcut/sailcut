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

#ifndef GEOCPP_SUBSPACE_H
#define GEOCPP_SUBSPACE_H

#include <geocpp/matrix.h>

enum subspaceflags_t { GEOCPP_FROM_BASE, GEOCPP_FROM_EQS };

/** Class for describing subspaces of an euclidian vector space,
 *  for example the empty space, points, lines and planes.
 */
class CSubSpace
{
private:
    CMatrix m;
    CVector p;
public:
    CSubSpace( unsigned int dim_space =3, unsigned int dim_sub =0 );
    CSubSpace(const CVector &pi, const CMatrix &mi, subspaceflags_t createflags = GEOCPP_FROM_EQS);
    
    /** Copy constructor */
    CSubSpace(const CSubSpace &s) : m(s.m) , p(s.p)
    {}
    ;
    CSubSpace intersect(const CSubSpace &) const;
    bool contains(const CVector &);
    
    /** Return the dimension of the whole space. */
    unsigned int getncoord() const
    {
        return p.getdim();
    }
    
    /** Return the subspace's dimension. */
    int getdim() const
    {
        if (p.getdim() > 0)
            return p.getdim()-m.getnrow();
        else
            return -1;
    }
    
    /** Accessor for the matrix. */
    const CMatrix& getm() const
    {
        return m;
    }
    
    /** Accessor for the point. */
    const CVector& getp() const
    {
        return p;
    }
};

// global functions
ostream& operator<< (ostream &, const CSubSpace &);

#endif
