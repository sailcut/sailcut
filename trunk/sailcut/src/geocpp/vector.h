/*
 * Copyright (C) 2002-2008 Jeremy Laine
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

#ifndef GEOCPP_VECTOR_H
#define GEOCPP_VECTOR_H

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// types and constants
typedef double real;

const real EPS = 1E-14;
const real PI = 3.14159265358979323846;

/** Real-valued vector
 */
class CVector : public vector<real>
{
public:
    CVector(size_t size) : vector<real>(size) {};
    CVector(const CVector &v) : vector<real>(v) {};

    real norm(void) const;
    CVector unit(void) const;

    bool operator== (const CVector& v) const;
    bool operator!= (const CVector& v) const;
    CVector operator+ (const CVector &) const;
    CVector operator- () const;
    CVector operator- (const CVector &) const;
    CVector operator* (const real &) const;
    real operator* (const CVector&) const;
};


// global functions
ostream& operator<<(ostream&, const CVector&);

/*********************************************
 
            Global functions
 
 *********************************************/

/** Binary '*' (multiply a scalar by a vector)
 */
inline
CVector operator*(const real& lambda, const CVector &v)
{
    return v * lambda;
}


#endif
