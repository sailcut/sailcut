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

#include <geocpp/vector.h>
#include <geocpp/matrix.h>
#include <geocpp/subspace.h>


/*******************************************************
 
               Member functions
 
********************************************************/

/** Returns the matrix corresponding to a vector
 */
CMatrix CVector::matrix(void) const
{
    CMatrix m(1,m_dim);
    for (unsigned int j = 0; j < m_dim; j++)
        m(0,j) = m_data[j];
    return m.transp();
}


/** Returns the vector's norm ("length")
 */
real CVector::norm(void) const
{
    real tot = 0;
    for (unsigned int i = 0; i < m_dim; i++)
        tot += m_data[i]*m_data[i];
    return real(sqrt(tot));
}


/** Returns corresponding unit length vector for non-zero vectors
 * and zero vector otherwise.
 */
CVector CVector::unit(void) const
{
    real n = norm();
    if (n<EPS)
        return CVector(m_dim);
    else
        return *this*(1/n);
}



/*********************************************
 
            Operators
 
 *********************************************/

/** Performs an assignment.
 */
CVector& CVector::operator=(const CVector& v)
{
    if (&v == this)
        return *this;

    if (m_dim != v.m_dim)
    {
        if (!isempty())
            delete [] m_data;

        m_dim = v.m_dim;

        if (v.isempty())
            m_data = NULL;
        else
            m_data = new real[m_dim];
    }

    if (!isempty())
        memcpy(m_data, v.m_data, sizeof(real) * m_dim);

    return *this;
}

/** Binary '+' operator (addition)
 */
CVector CVector::operator+(const CVector& v2) const
{
    if (m_dim != v2.m_dim)
        throw CException("VectTempl::operator+ : dimension mismatch!");

    CVector ret(m_dim);
    for (unsigned int i=0; i < m_dim; i++)
        ret.m_data[i] = m_data[i] + v2.m_data[i];
    return ret;
}


/** Unary '-' operator (return opposite)
 */
CVector CVector::operator-() const
{
    CVector ret(m_dim);
    for (unsigned i = 0; i < m_dim; i++)
        ret.m_data[i] = - m_data[i];
    return ret;
}


/** Binary '-' operator (return difference)
 */
CVector CVector::operator-(const CVector& v2) const
{
    if (m_dim != v2.m_dim)
        throw CException("CVector::operator- : dimension mismatch!");

    CVector ret(m_dim);
    for (unsigned int i=0; i < m_dim; i++)
        ret.m_data[i] = m_data[i] - v2.m_data[i];
    return ret;
}


/** Binary '* (multiply a vector by a real)
 */
CVector CVector::operator*(const real& lambda) const
{
    CVector ret(m_dim);
    for (unsigned i = 0; i < m_dim; i++)
        ret.m_data[i] = m_data[i]* lambda;

    return ret;
}


/** Tests vectors for equality.
 */
bool CVector::operator==(const CVector &v) const
{
    if (m_dim != v.m_dim)
        return false;

    for (unsigned int i = 0; i < m_dim; i++)
        if (fabs(m_data[i] - v.m_data[i]) > EPS)
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
    if (m_dim != v2.m_dim)
        throw CException("CVector::operator*: dimension mismatch!");

    real ret = 0;
    for (unsigned int i=0; i < m_dim; i++)
        ret += m_data[i] * v2.m_data[i];
    return ret;
}


/*********************************************
 
            Global functions
 
 *********************************************/

/** Outputs a CVector to a stream.
 */
ostream& operator<<(ostream &o, const CVector &v)
{
    if (v.getdim() >0)
    {
        o << v.getcoord(0);
        for (unsigned int i=1; i < v.getdim(); i++)
            o << "\t" << v.getcoord(i);
    }
    return o;
}
