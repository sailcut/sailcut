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

using namespace std;

// types and constants
typedef double real;

const real EPS = 1E-14;
const real PI = 3.14159265358979323846;

class CMatrix;

/** Real-valued vector
 */
class CVector
{
protected:
    /** the vector's dimension */
    size_t m_dim;
    /** the vector's coordinates */
    real * m_data;
public:
    // construction / destruction
    CVector(size_t size = 0);
    CVector(const CVector &v);
    /** The destructor. */
    ~CVector()
    {
        if (m_dim>0)
            delete [] m_data;
    }


    // member functions
    /** Returns the dimension of the vector. */
    size_t size() const
    {
        return m_dim;
    }

    /** Is this a null-dimension vector ?*/
    bool empty() const
    {
        return !m_dim;
    }
    CMatrix matrix() const;
    real norm(void) const;
    CVector unit(void) const;

    // operators
    CVector & operator= (const CVector &);
    bool operator== (const CVector& v) const;
    bool operator!= (const CVector& v) const;
    CVector operator+ (const CVector &) const;
    CVector operator- () const;
    CVector operator- (const CVector &) const;
    CVector operator* (const real &) const;
    real operator* (const CVector&) const;
    real & operator[] (size_t index);
    real operator[] (size_t index) const;

    // friend classes
    friend class CMatrix;
    friend class CSubSpace;
    friend class CVector2d;
    friend class CVector3d;
};


// global functions
ostream& operator<<(ostream&, const CVector&);


/*********************************************
 
            Construction & destruction
 
 *********************************************/

/** Constructs a CVector vector with the given dimension
 */
inline
CVector::CVector(size_t size)
        : m_dim(size)
{
    // if size is non-zero, we allocate memory
    if (!empty())
    {
        m_data = new real[m_dim];
        memset(m_data, 0, sizeof(real) * m_dim);
    }
    else
    {
        m_data = NULL;
    }
}

/** Copy constructor.
 */
inline
CVector::CVector(const CVector& v)
        : m_dim(v.m_dim)
{
    // if size is non-zero, we allocate memory
    if (!empty())
    {
        m_data = new real[m_dim];
        memcpy(m_data, v.m_data, sizeof(real) * m_dim);
    }
    else
    {
        m_data = NULL;
    }
}

/*********************************************
 
            Operators
 
 *********************************************/



/** Returns the index'th coordinate of a vector.
 */
inline
real& CVector::operator[] (size_t index)
{
    if (index >= m_dim)
        throw range_error("CVector::operator[] : out of bounds!");
    return m_data[index];
}


/** Returns the index'th coordinate of a vector.
 */
inline
real CVector::operator[] (size_t index) const
{
    if (index >= m_dim)
        throw range_error("CVector::operator[] : out of bounds!");
    return m_data[index];
}


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
