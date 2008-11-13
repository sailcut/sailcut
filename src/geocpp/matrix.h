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

#ifndef GEOCPP_MATRIX_H
#define GEOCPP_MATRIX_H

#include <geocpp/vector.h>

enum soltype_t { NONE, ONE, INF };

class CSubSpace;

/** Class for describing matrices.
 *
 * @ingroup GeoCpp
 */
class CMatrix
{
protected:
    /** the matrix's data */
    real* m_data;
    /** number of rows */
    unsigned int m_nrow;
    /** number of columns */
    unsigned int m_ncol;

public:
    CMatrix(const CMatrix &m);
    CMatrix(const unsigned int& = 0, const unsigned int& = 0);
    /** The destructor.
     */
    ~CMatrix()
    {
        if (!isempty())
            delete [] m_data;
    };

public:
    // static functions
    static CMatrix id(const unsigned int&);
    static CMatrix rnd(const unsigned int&, const unsigned int&);
    static CMatrix rot3d(const unsigned int&, const real&);

    // member functions
    CVector col(const unsigned int&) const;
    CMatrix   crop(const unsigned int& nr, const unsigned int& nc, const unsigned int& nrz=0, const unsigned int& ncz=0) const;
    real      det(void) const;
    CMatrix   dev(const unsigned int&, const unsigned int&) const;
    /** Diagonalises matrix. Currently this is implemented as
     * a full pivot Gauss-Jordan diagonalisation.
     */
    CMatrix diag() const
    {
        return gaussjordan();
    };
    CMatrix gaussjordan(bool *is_inv=NULL, CMatrix *inv=NULL, soltype_t *soltype=NULL, CVector *bb=NULL, CMatrix *tkern=NULL) const;
    /** Accessor for the number of columns. */
    unsigned int getncol() const
    {
        return m_ncol;
    }
    /** Accessor for the number of rows. */
    unsigned int getnrow() const
    {
        return m_nrow;
    }
    CMatrix   img(void) const;
    CMatrix   inv(void) const;
    /** Is the matrixan empty (0x0) matrix? */
    bool isempty() const
    {
        return ! (m_ncol && m_nrow);
    }
    CMatrix   kern(const unsigned int& vsize) const;
    CVector row(unsigned int) const;
    CSubSpace solve(const CVector &) const;
    void      swap_row(const unsigned int&, const unsigned int&);
    void      swap_col(const unsigned int&, const unsigned int&);
    CMatrix   transp(void) const;

    // operators
    real& operator() ( const unsigned int& row, const unsigned int& col );
    real operator() ( const unsigned int& row, const unsigned int& col ) const;
    CMatrix&  operator=(const CMatrix &);
    bool      operator==(const CMatrix &) const;
    bool      operator!=(const CMatrix &) const;
    CMatrix   operator-() const;
    CMatrix   operator*(const CMatrix &) const;
    CVector operator*(const CVector &) const;
};


// global functions
ostream& operator<< (ostream &, const CMatrix &);

// inlines

/** Copy constructor.
 */
inline
CMatrix::CMatrix(const CMatrix &m)
        : m_nrow(m.m_nrow), m_ncol(m.m_ncol)
{
    if (!isempty())
    {
        m_data = new real[m_nrow * m_ncol];
        memcpy(m_data, m.m_data, sizeof(real) * m_nrow * m_ncol);
    }
    else
    {
        m_data = NULL;
    }
}


/** Constructs a CMatrix with the given number of rows and columns.
 */
inline
CMatrix::CMatrix(const unsigned int& nrow, const unsigned int& ncol)
        : m_nrow(nrow), m_ncol(ncol)
{
    if (!isempty())
    {
        m_data = new real[m_nrow * m_ncol];
        memset(m_data, 0, sizeof(real) * m_nrow * m_ncol);
    }
    else
    {
        m_data = NULL;
    }
}


/** Returns the selected element of the matrix.
 */
inline
real& CMatrix::operator() (const unsigned int& row, const unsigned int& col)
{
    if (row >= m_nrow || col >= m_ncol)
        throw range_error("CMatrix::operator() : index out of bounds");
    return m_data[m_ncol * row + col];
}


/** Returns the selected element of the matrix.
 */
inline
real CMatrix::operator() (const unsigned int& row, const unsigned int& col) const
{
    if (row >= m_nrow || col >= m_ncol)
        throw range_error("CMatrix::operator() : index out of bounds");
    return m_data[m_ncol * row + col];
}

#endif
