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

#ifndef GEOCPP_MATRIX_H
#define GEOCPP_MATRIX_H

#include <cstring>
#include <stdexcept>

#include <geocpp/vector.h>

enum soltype_t { NONE, ONE, INF };


/** Class for describing matrices.
 *
 * @ingroup GeoCpp
 */
class CMatrix
{
public:

    /** Constructs a CMatrix with the given number of rows and columns.
    */
    CMatrix(const size_t &nrow = 0, const size_t &ncol = 0)
        : m_nrow(nrow), m_ncol(ncol)
    {
        if (!empty())
        {
            m_data = new real[m_nrow * m_ncol];
            memset(m_data, 0, sizeof(real) * m_nrow * m_ncol);
        }
        else
        {
            m_data = NULL;
        }
    };


    /** Copy constructor.
    */
    CMatrix(const CMatrix &m)
            : m_nrow(m.m_nrow), m_ncol(m.m_ncol)
    {
        if (!empty())
        {
            m_data = new real[m_nrow * m_ncol];
            memcpy(m_data, m.m_data, sizeof(real) * m_nrow * m_ncol);
        }
        else
        {
            m_data = NULL;
        }
    };

    /** The destructor.
     */
    ~CMatrix()
    {
        if (!empty())
            delete [] m_data;
    };

    // static functions
    static CMatrix id(const size_t&);
    static CMatrix rnd(const size_t&, const size_t&);
    static CMatrix rot3d(const size_t&, const real&);

    // member functions
    CMatrix   crop(const size_t& nr, const size_t& nc, const size_t& nrz=0, const size_t& ncz=0) const;
    real      determinant() const;
    CMatrix   dev(const size_t&, const size_t&) const;

    /** Accessor for the number of columns. */
    size_t columns() const
    {
        return m_ncol;
    }
    /** Accessor for the number of rows. */
    size_t rows() const
    {
        return m_nrow;
    }
    /** Is the matrixan empty (0x0) matrix? */
    bool empty() const
    {
        return ! (m_ncol && m_nrow);
    }

    CMatrix gaussjordan(bool *is_inv=NULL, CMatrix *inv=NULL, soltype_t *soltype=NULL, CVector *bb=NULL, CMatrix *tkern=NULL) const;
    CMatrix   kern(const size_t& vsize) const;
    CMatrix   transposed() const;

    // operators
    real& operator() ( const size_t& row, const size_t& col );
    real operator() ( const size_t& row, const size_t& col ) const;
    CMatrix&  operator=(const CMatrix &);
    bool      operator==(const CMatrix &) const;
    bool      operator!=(const CMatrix &) const;
    CMatrix   operator-() const;
    CMatrix   operator*(const CMatrix &) const;
    CVector operator*(const CVector &) const;

private:
    CVector col(size_t) const;
    CVector row(size_t) const;
    void swap_row(size_t, size_t);
    void swap_col(size_t, size_t);

    /** the matrix's data */
    real* m_data;
    /** number of rows */
    size_t m_nrow;
    /** number of columns */
    size_t m_ncol;
};


// inlines

/** Returns the selected element of the matrix.
 */
inline
real& CMatrix::operator() (const size_t& row, const size_t& col)
{
#ifdef CHECK_DIMENSIONS
    if (row >= m_nrow || col >= m_ncol)
        throw range_error("CMatrix::operator() : index out of bounds");
#endif
    return m_data[m_ncol * row + col];
}


/** Returns the selected element of the matrix.
 */
inline
real CMatrix::operator() (const size_t& row, const size_t& col) const
{
#ifdef CHECK_DIMENSIONS
    if (row >= m_nrow || col >= m_ncol)
        throw range_error("CMatrix::operator() : index out of bounds");
#endif
    return m_data[m_ncol * row + col];
}

#endif
