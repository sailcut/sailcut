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

#ifdef CHECK_DIMENSIONS
#include <stdexcept>
#endif

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

    /** Returns the vector's length.
     */
    real length() const
    {
        real tot = 0;
        for (size_t i = 0; i < size(); i++)
            tot += (*this)[i]*(*this)[i];
        return real(sqrt(tot));
    };

    /** Returns corresponding unit length vector for non-zero vectors
     * and zero vector otherwise.
     */
    CVector normalized(void) const
    {
        const real n = length();
        if (n<EPS)
            return CVector(size());
        else
            return *this*(1/n);
    };

    /** Tests vectors for equality.
    */
    bool operator==(const CVector &v) const
    {
#ifdef CHECK_DIMENSIONS
        if (size() != v.size())
            throw invalid_argument("CVector::operator== : dimension mismatch!");
#endif
        for (size_t i = 0; i < size(); i++)
            if (fabs((*this)[i] - v[i]) > EPS)
                return false;
        return true;
    };

    /** Tests vectors for non-equality.
     */
    bool operator!=(const CVector &v) const
    {
        return !(*this == v);
    };

    /** Binary '+' operator (addition)
    */
    CVector operator+(const CVector& v2) const
    {
#ifdef CHECK_DIMENSIONS
        if (size() != v2.size())
            throw invalid_argument("CVector::operator+ : dimension mismatch!");
#endif
        CVector ret(*this);
        for (size_t i = 0; i < size(); i++)
            ret[i] += v2[i];
        return ret;
    };

    /** Unary '-' operator (return opposite)
     */
    CVector operator- () const
    {
        CVector ret(size());
        for (size_t i = 0; i < size(); i++)
            ret[i] = - (*this)[i];
        return ret;
    };

    /** Binary '-' operator (return difference)
    */
    CVector operator- (const CVector &v2) const
    {
#ifdef CHECK_DIMENSIONS
        if (size() != v2.size())
            throw invalid_argument("CVector::operator- : dimension mismatch!");
#endif
        CVector ret(*this);
        for (size_t i = 0; i < size(); i++)
            ret[i] -= v2[i];
        return ret;
    };

    /** Binary '* (multiply a vector by a real)
    */
    CVector operator*(const real& lambda) const
    {
        CVector ret(*this);
        for (size_t i = 0; i < size(); i++)
            ret[i] *= lambda;
        return ret;
    };
};

/** Outputs a CVector to a stream.
 */
inline
ostream& operator<<(ostream &o, const CVector &v)
{
    for (size_t i = 0; i < v.size(); i++)
        o << (i > 0 ? "\t" : "") << v[i];
    return o;
}

/** Binary '*' (multiply a scalar by a vector)
 */
inline
CVector operator*(const real& lambda, const CVector &v)
{
    return v * lambda;
}


#endif
