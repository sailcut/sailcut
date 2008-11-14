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

#include <cmath>

#include <geocpp/vector.h>


/*******************************************************

               Member functions

********************************************************/


/** Returns the vector's norm ("length")
 */
real CVector::norm(void) const
{
    real tot = 0;
    for (size_t i = 0; i < size(); i++)
        tot += (*this)[i]*(*this)[i];
    return real(sqrt(tot));
}


/** Returns corresponding unit length vector for non-zero vectors
 * and zero vector otherwise.
 */
CVector CVector::unit(void) const
{
    real n = norm();
    if (n<EPS)
        return CVector(size());
    else
        return *this*(1/n);
}

/*********************************************

            Operators

 *********************************************/

/** Binary '+' operator (addition)
 */
CVector CVector::operator+(const CVector& v2) const
{
    if (size() != v2.size())
        throw invalid_argument("VectTempl::operator+ : dimension mismatch!");

    CVector ret(*this);
    for (size_t i = 0; i < size(); i++)
        ret[i] += v2[i];
    return ret;
}


/** Unary '-' operator (return opposite)
 */
CVector CVector::operator-() const
{
    CVector ret(size());
    for (size_t i = 0; i < size(); i++)
        ret[i] = - (*this)[i];
    return ret;
}


/** Binary '-' operator (return difference)
 */
CVector CVector::operator-(const CVector& v2) const
{
    if (size() != v2.size())
        throw invalid_argument("CVector::operator- : dimension mismatch!");

    CVector ret(*this);
    for (size_t i = 0; i < size(); i++)
        ret[i] -= v2[i];
    return ret;
}


/** Binary '* (multiply a vector by a real)
 */
CVector CVector::operator*(const real& lambda) const
{
    CVector ret(*this);

    for (size_t i = 0; i < size(); i++)
        ret[i] *= lambda;

    return ret;
}


/** Tests vectors for equality.
 */
bool CVector::operator==(const CVector &v) const
{
    if (size() != v.size())
        return false;

    for (size_t i = 0; i < size(); i++)
        if (fabs((*this)[i] - v[i]) > EPS)
            return false;
    return true;
}


/** Tests vectors for non-equality.
 */
bool CVector::operator!=(const CVector &v) const
{
    return !(*this == v);
}


/** Dot (real) product.
 */
real CVector::operator*(const CVector &v2) const
{
    if (size() != v2.size())
        throw invalid_argument("CVector::operator*: dimension mismatch!");

    real ret = 0;
    for (size_t i = 0; i < size(); i++)
        ret += (*this)[i] * v2[i];
    return ret;
}


/*********************************************

            Global functions

 *********************************************/

/** Outputs a CVector to a stream.
 */
ostream& operator<<(ostream &o, const CVector &v)
{
    if (v.size() > 0)
    {
        o << v[0];
        for (size_t i=1; i < v.size(); i++)
            o << "\t" << v[i];
    }
    return o;
}
