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

#include <geocpp/matrix.h>
#include <geocpp/subspace.h>

#include <climits>
#include <cmath>
#include <cstdlib>

/** Returns the matrix corresponding to a vector
 */
CMatrix::CMatrix(const CVector &v)
    : m_nrow(v.size()), m_ncol(1)
{
    if (v.size() > 0)
    {
        m_data = new real[v.size()];
        for (size_t j = 0; j < v.size(); j++)
            m_data[j] = v[j];
    }
    else
    {
        m_data = NULL;
    }
}

/*******************************************************

                      Static functions

********************************************************/

/** Returns the (square) identity matrix for a given dimension.
 *
 * @param n the dimension of the identity matrix.
 */
CMatrix CMatrix::id(const size_t& n)
{
    CMatrix m = CMatrix(n,n);
    for (size_t i = 0; i < n; i++)
        m(i,i) = 1;
    return m;
}


/** Returns a random matrix of specified dimension.
 *
 * @param nrow number of rows for the matrix
 * @param ncol number of columns for the matrix
 */
CMatrix CMatrix::rnd(const size_t& nrow, const size_t& ncol)
{
    CMatrix m(nrow,ncol);
    for (size_t i=0; i < nrow; i++)
    {
        for (size_t j=0; j < ncol; j++)
        {
            m(i,j) = (100 * real(rand()) / RAND_MAX)-50;
        }
    }
    return m;
}


/** Return a rotation matrix about specified axis.
 *
 * @param axis the index (0,1,..) of the axis
 * @param angle rotation angle in radians
 */
CMatrix CMatrix::rot3d(const size_t& axis, const real& angle)
{
    CMatrix m(3,3);

    // build rotation matrix around x axis
    m( axis, axis ) = 1;
    m( (axis+1)%3, (axis+1)%3 ) = cos(angle);
    m( (axis+2)%3, (axis+2)%3 ) = cos(angle);
    m( (axis+1)%3, (axis+2)%3 ) = -sin(angle);
    m( (axis+2)%3, (axis+1)%3 ) = sin(angle);

    return m;
}


/*******************************************************

                      Member functions

********************************************************/

/** Extracts the vector for the specified column.
 *
 * @param index the index of the column to return
 */
CVector CMatrix::col(const size_t& index) const
{
    if (index >= m_ncol)
        throw range_error("CMatrix::col : index out of bounds!");

    CVector ret(m_nrow);
    for (size_t i = 0; i < m_nrow; i++)
        ret[i] = m_data[i*m_ncol + index];
    return ret;
}


/** Extracts part of a CMatrix.
 *
 * @param nr row to start at
 * @param nc column to start at
 * @param nrz number of rows to extract
 * @param ncz number of columns to extract
 */
CMatrix CMatrix::crop(const size_t& nr,
                      const size_t& nc,
                      const size_t& nrz,
                      const size_t& ncz) const
{
    CMatrix ret(nr,nc);
    size_t nrm,ncm;
    if (nr < (m_nrow - nrz))
        nrm = nr;
    else
        nrm = m_nrow - nrz;

    if (nc < (m_ncol - ncz))
        ncm = nc;
    else
        ncm = m_ncol - ncz;

    for (size_t i=0; i < nrm; i++)
        for (size_t j=0; j < ncm; j++)
            ret(i,j) = (*this)(i,j+ncz);
    return ret;
}


/** Returns the determinant of a CMatrix. The matrix must be square!
 */
real CMatrix::det(void) const
{
    if (m_nrow != m_ncol)
        throw invalid_argument("CMatrix::det : matrix is not square!");

    real ret=0;
    // if we have a scalar matrix return its only coefficient
    if (m_nrow == 1)
        return (*this)(0,0);

    // develop relative to first row
    for (size_t j=0; j < m_ncol; j++)
    {
        if (j%2)
            ret -= (*this)(0,j) * dev(0,j).det();
        else
            ret += (*this)(0,j) * dev(0,j).det();
    }
    return ret;
}


/** Returns a square Matrix with the specified  rows and lines removed.
 * The matrix must be square!
 *
 * @param i the index of the row to remove
 * @param j the index of the column to remove
 */
CMatrix CMatrix::dev(const size_t& i, const size_t& j) const
{
    if ((i >= m_nrow) || (j >= m_ncol))
        throw range_error("CMatrix::dev : index out of bounds");

    if (m_nrow != m_ncol)
        throw invalid_argument("CMatrix::dev : matrix is not square!");

    size_t dx=0,dy=0;

    if (empty())
        return CMatrix();

    CMatrix m(m_nrow - 1, m_ncol - 1);
    for (size_t x=0; x < m_nrow; x++)
    {
        if (x!=i)
        {
            dy = 0;
            for (size_t y=0; y < m_ncol; y++)
            {
                if (y!=j)
                {
                    m(dx,dy) =(*this)(x,y);
                    dy++;
                }
            }
            dx++;
        }
    }
    return m;
}


/** Diagonalises matrix by Gauss-Jordan method with full pivoting
 * optionally returns the inverse matrix.
 */
CMatrix CMatrix::gaussjordan(bool *is_inv, CMatrix *inv, soltype_t * soltype, CVector *bb, CMatrix *tkern) const
{
    // check dimensions
    CMatrix b;
    if (bb!=NULL)
    {
        if (bb->size() != m_nrow)
            throw invalid_argument("CMatrix::solve : matrix <=> right-hand side dimensions incompatible");

        b = *bb;
    }
    else
    {
        b = CMatrix(m_nrow, 1);
    }
    //cout << "b" << endl<< b << endl;
    //cout << "m" << endl<< *this << endl;

    real p_value;
    real p_avalue, avalue;
    size_t p_i, p_j;
    size_t i,j,k;
    size_t n;
    // copy over the matrix
    CMatrix m = *this;
    CMatrix t;
    // left-hand operations (row ops)
    CMatrix lops = CMatrix::id(m_nrow);
    //  CMatrix rswap = CMatrix::id(getnrow());
    // right-hand operations (col ops)
    CMatrix cswap = CMatrix::id(m_ncol);

    // determine minimum dimension
    if (m_ncol < m_nrow)
        n = m_ncol;
    else
        n = m_nrow;


    ////////////////////////////////
    //     DIAGONALISATION        //
    ////////////////////////////////

    for (k=0; k < n; k++)
    {
        // find pivot
        p_value = p_avalue = 0;
        p_i = p_j = 0;
        for (i = k; i < m_nrow; i++)
        {
            for (j = k; j < m_ncol; j++)
            {
                avalue = fabs(m(i,j));
                if (avalue > p_avalue)
                {
                    p_value = m(i,j);
                    p_avalue = avalue;
                    p_i = i;
                    p_j = j;
                }
            }
        }
        // if pivot is smaller than epsillon we cannot continue diagonalisation
        if ( p_avalue < EPS )
            break;

        // exchange rows k and p_i
        // swapping
        m.swap_row(k,p_i);
        lops.swap_row(k,p_i);
        //rswap.swap_row(k,p_i);
        b.swap_row(k,p_i);
        // swapping cols k and p_j
        m.swap_col(k,p_j);
        cswap.swap_col(k,p_j);
        // modify rows
        t = CMatrix::id(m_nrow);
        for (i =0; i < m_nrow; i++)
        {
            if ( i !=k )
                t(i,k) = -m(i,k) / p_value;
            else
                t(k,k) =  1 / p_value;
        }
        m = t * m;
        b = t * b;
        lops = t * lops;
    }


    /////////////////////////////////////
    //  HANDLE REQUEST FOR INVERSION   //
    /////////////////////////////////////

    if ((is_inv != NULL) && (inv != NULL))
    {
        if ((m_nrow == m_ncol)&&(m_ncol == k))
        {
            // matrix is square and invertible
            *inv = cswap*lops;
            *is_inv = true;
        }
    }


    /////////////////////////////////////
    //  HANDLE REQUEST FOR SOLUTION    //
    /////////////////////////////////////

    if ((bb != NULL)&&(soltype != NULL))
    {
        *soltype = ONE;
        for (i = k; i < b.m_nrow; i++)
            if (b(i,0) > EPS)
            {
                *soltype = NONE;
                break;
            }

        if (*soltype==NONE)
        {
            // incompatible system
            *bb = CVector(0);
        }
        else
        {
            // system compatible one or infinity of solutions
            *bb = (cswap * b.crop(m_ncol,1)).col(0);
            if (m_ncol == k)
            {
                // complete diagonalisation, one solution
                *soltype=ONE;
            }
            else
            {
                *soltype=INF;
            }
        }
    }

    /////////////////////////////////////
    //  HANDLE REQUEST FOR KERNEL     //
    /////////////////////////////////////

    if (tkern != NULL)
    {
        if (k < m_ncol)
        {
            /*
                  cout << "has non-zero kernel" << endl;
                  if ((getnrow()==k)) {
                    // no redundant equations
                    cout << "getnrow()==k==" << k << endl;
                  } else {
                    cout << "getnrow()==" << getnrow() << " k==" << k << endl;
                  }
            */
            CMatrix pk = - m.crop(m_ncol-k+1,m_ncol-k,0,k);
            pk = pk.crop(m_ncol,m_ncol-k);
            for (i=0; i + k < m_ncol; i++)
                pk(i+k,i) = 1;
            *tkern = cswap* pk;
        }
        else
        {
            // matrix is inversible
            //cout << "m_ncol==k" << endl;
            *tkern = CMatrix();
        }
    }
    // clean matrix

    for (i = 0; i < m_nrow; i++)
        for (j = 0; j < m_ncol; j++)
            if (fabs(m(i,j)) < EPS)
                m(i,j) = 0;

    //  return m;

    return m.crop(k,m_ncol);
}


/** Returns the image space of the linear application
 *  associated with the matrix.
 */
CMatrix CMatrix::img(void) const
{
    return transp().diag().transp();
}


/** Inverts matrix by Gauss-Jordan method with full pivoting.
 */
CMatrix CMatrix::inv(void) const
{
    if (m_nrow != m_ncol)
        throw invalid_argument("CMatrix:: inv : matrix is not square !");
 
    // if matrix is empty, return empy matrix
    if (empty())
        return CMatrix();

    CMatrix ret;
    bool is_inv;
    gaussjordan(&is_inv,&ret);
    if (is_inv == false)
        throw invalid_argument("CMatrix::diag : matrix is singular !");

    return ret;
}


/** Returns the kernel of the linear application.
 *  associated with the matrix.
 */
CMatrix CMatrix::kern(const size_t& vsize) const
{
    if (empty())
    {
        cout << "REQUEST FOR KER(o)" << endl;
        return CMatrix::id(vsize);
    }

    CMatrix ret;
    gaussjordan(NULL,NULL,NULL,NULL,&ret);
    return ret;
}


/** Retrieves a row of the matrix in vector form.
 */
CVector CMatrix::row(size_t index) const
{
    if (index >= m_nrow)
        throw range_error("CMatrix::row : index out of bounds!");

    CVector ret(m_ncol);
    for (size_t i = 0; i < m_ncol; i++)
        ret[i] = m_data[index*m_ncol + i];
    return ret;
}


/** Solves the linear equations system formed by the current matrix
 * and a given right-hand side vector.
 *
 * @param b the right-hand side values in the equation
 */
CSubSpace CMatrix::solve(const CVector &b) const
{
    if (m_nrow != b.size())
        throw invalid_argument("CMatrix::solve : dimension mismatch");

    //cout << "[[ solver ]]" << endl;
    //cout << "m" << endl << *this << endl;
    //cout << "b" << endl << b << endl;

    soltype_t soltype=NONE;
    CVector s = b;
    CMatrix k;
    CSubSpace h(CVector(0),CMatrix());

    gaussjordan(NULL,NULL,&soltype,&s,&k);
    switch (soltype)
    {
    case NONE:
        //cout << "CMatrix::solve : system has no solution" << endl;
        break;
    case ONE:
        //cout << "CMatrix::solve : system has unique solution" << endl;
        h = CSubSpace(s,CMatrix::id(s.size()));
        break;
    case INF:
        //cout << "CMatrix::solve : system has an infinity of solutions" << endl;
        h = CSubSpace(s,k,GEOCPP_FROM_BASE);
        break;
    }
    return h;
}


/** Swaps two rows of a matrix.
 */
void CMatrix::swap_row(const size_t& i1, const size_t& i2)
{
    if ((i1>=m_nrow)||(i2>=m_nrow))
        throw range_error("CMatrix:swap_row : index out of bounds");

    if (i1==i2)
        return;

    real temp;
    size_t pos1 = i1 * m_ncol;
    size_t pos2 = i2 * m_ncol;
    for (size_t j = 0; j < m_ncol; j++)
    {
        temp = m_data[pos1+j];
        m_data[pos1+j] = m_data[pos2+j];
        m_data[pos2+j] = temp;
    }
}


/** Swap two columns of a matrix.
 */
void CMatrix::swap_col(const size_t& j1, const size_t& j2)
{
    if ((j1>=m_ncol)||(j2>=m_ncol))
        throw range_error("CMatrix:swap_col : index out of bounds");

    if (j1==j2)
        return;

    real temp;
    size_t pos = 0;
    for (size_t i = 0; i < m_nrow; i++)
    {
        temp = m_data[pos+j1];
        m_data[pos+j1] = m_data[pos+j2];
        m_data[pos+j2] = temp;
        pos += m_ncol;
    }
}


/** Transposes ('tilts') matrix
 */
CMatrix CMatrix::transp(void) const
{
    CMatrix ret(m_ncol,m_nrow);

    for (size_t i=0; i < m_ncol; i++)
        for (size_t j=0; j< m_nrow; j++)
            ret.m_data[i*ret.m_ncol + j] = m_data[j*m_ncol + i];
    return ret;
}


/*******************************************************

                      Operators

********************************************************/


/** Performs an assignment.
 */
CMatrix & CMatrix::operator=(const CMatrix &m)
{
    if (&m == this)
        return *this;

    if ( (m_nrow != m.m_nrow) || (m_ncol != m.m_ncol) )
    {
        if (m_data)
            delete [] m_data;

        m_nrow = m.m_nrow;
        m_ncol = m.m_ncol;

        if (m.m_data)
            m_data = new real[m_nrow * m_ncol];
        else
            m_data = NULL;
    }

    if (m_data)
        memcpy(m_data, m.m_data, sizeof(real) * m_nrow * m_ncol);

    return *this;
}


/** Tests two matrices for equality.
 */
bool CMatrix::operator==(const CMatrix &m) const
{
    if ( (m_nrow != m.m_nrow) || (m_ncol != m.m_ncol) )
        return false;

    for (size_t i=0; i < m_nrow; i++)
        for (size_t j=0; j < m_ncol; j++)
            if ( (*this)(i,j) != m(i,j) )
                return false;

    return true;
}


/** Tests two matrices for non-equality.
 */
bool CMatrix::operator!=(const CMatrix &m) const
{
    return !(*this == m);
}


/** Return the opposite of a matrix.
 */
CMatrix CMatrix::operator-() const
{
    CMatrix ret(m_nrow,m_ncol);

    for (size_t pos = 0; pos < m_nrow * m_ncol; pos++)
        ret.m_data[pos]  = - m_data[pos];
    return ret;
}

/** Matrix product
 */
CMatrix CMatrix::operator*(const CMatrix &m2) const
{
    if (m_ncol != m2.m_nrow)
        throw invalid_argument("CMatrix::operator*: dimension mismatch!");

    CMatrix p(m_nrow, m2.m_ncol);
    size_t pos = 0;
    for (size_t i = 0; i < p.m_nrow; i++)
        for (size_t j = 0; j < p.m_ncol; j++)
        {
            for (size_t k=0; k < m_ncol; k++)
                p.m_data[pos] += m2.m_data[m2.m_ncol * k + j] * m_data[m_ncol * i + k];

            pos++;
        }

    return p;
}


/** Multiplies a matrix by a vector.
 */
CVector CMatrix::operator*(const CVector &v) const
{
    if (m_ncol != v.size())
        throw invalid_argument("CMatrix::operator*: dimension mismatch!");

    // result is initialised to zero
    CVector p(m_nrow);
    size_t pos = 0;
    for (size_t i = 0; i < m_nrow; i++)
    {
        for (size_t k=0; k < m_ncol; k++)
        {
            p[i] += m_data[pos] * v[k];
            pos++;
        }
    }
    return p;
}

