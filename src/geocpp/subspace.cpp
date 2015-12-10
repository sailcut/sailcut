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

#define SIZE 3

#include <geocpp/subspace.h>


static CVector toVector(const CVector3d &v)
{
    CVector vv(3);
    vv[0] = v.x();
    vv[1] = v.y();
    vv[2] = v.z();
    return vv;
}


/*******************************************************

               Construction / destruction

********************************************************/

/** Constructs an empty CSubSpace.
 */
CSubSpace::CSubSpace()
    : isEmpty(true)
{
}


/** Constructs a CSubSpace from a point and either the equations or
 * a base of the subspace.
 *
 * @param pi a point in the subspace
 * @param mi a matrix containing the equations or the base of the subspace
 * @param createflags specifies whether we were given the equations or the base of the subspace
 */
CSubSpace::CSubSpace(const CVector3d &pi, const CMatrix &mi, subspaceflags_t createflags)
    : isEmpty(false)
    , p(pi)
{
    switch ( createflags )
    {
    case GEOCPP_FROM_EQS:
        // equations are given in lines
        if ( (mi.rows() > 0) && (mi.columns() != SIZE) )
            throw invalid_argument("CSubSpace::CSubSpace(p,m,GEOCPP_FROM_EQS) : dimension mismatch between p and m");

        m = mi;
        break;
    case GEOCPP_FROM_BASE:
        // base is given in column format
        if ( (mi.columns() > 0) && (mi.rows() != SIZE) )
            throw invalid_argument("CSubSpace::CSubSpace(p,m,GEOCPP_FROM_BASE) : dimension mismatch between p and m");

        m = mi.transposed().kern(SIZE).transposed();
        break;
    default:
        throw invalid_argument("CSubSpace::CSubspace(p,m,createflags) : unknown creation flags");
    }
}


/** Copy constructor */
CSubSpace::CSubSpace(const CSubSpace &s)
    : isEmpty(s.isEmpty)
    , m(s.m)
    , p(s.p)
{
}


/** Test whether the CSubSpace contains a given point.
 */
bool CSubSpace::contains(const CVector3d &point) const
{
    vector<real> prod = m * toVector(point-p);
    real lengthSquared = 0;
    for (size_t i = 0; i < prod.size(); ++i)
        lengthSquared += prod[i] * prod[i];
    return sqrt(lengthSquared) < EPS;
}


/** Return the subspace's dimension. */
int CSubSpace::getdim() const
{
    if (isEmpty)
        return -1;
    else
        return SIZE - m.rows();
}

/** Performs the intersection of two CSubSpace objects.
 */
CSubSpace CSubSpace::intersect(const CSubSpace &h2) const
{
    if (isEmpty || h2.isEmpty)
        return CSubSpace();

    CVector b1 = m * toVector(p);
    CVector b2 = h2.m * toVector(h2.p);
    CVector bb( m.rows() + h2.m.rows() );
    CMatrix mm( m.rows() + h2.m.rows(), SIZE );
    for (size_t i = 0 ; i < mm.rows() ; i++)
    {
        if ( i < m.rows() )
        {
            for (size_t j = 0 ; j < mm.columns() ; j++)
                mm(i,j) = m(i,j);
            bb[i] = b1[i];
        }
        else
        {
            for (size_t j = 0 ; j < mm.columns() ; j++)
                mm(i,j) = h2.m(i - m.rows(), j);
            bb[i] = b2[i - m.rows()];
        }
    }
    //cout << "mm" << endl << mm << endl;
    //cout << "bb" << endl << bb << endl;

    soltype_t soltype = NONE;
    CMatrix k;

    mm.gaussjordan(NULL, NULL, &soltype, &bb, &k);

    CVector3d s;
    for (size_t i = 0; i < qMin(size_t(3), bb.size()); ++i)
        s[i] = bb[i];

    switch (soltype)
    {
    case ONE:
        //cout << "CMatrix::solve : system has unique solution" << endl;
        return CSubSpace(s, CMatrix::id(SIZE), GEOCPP_FROM_EQS);
    case INF:
        //cout << "CMatrix::solve : system has an infinity of solutions" << endl;
        return CSubSpace(s, k, GEOCPP_FROM_BASE);
    case NONE:
        //cout << "CMatrix::solve : system has no solution" << endl;
        break;
    }
    return CSubSpace();
}


CVector3d CSubSpace::intersectionPoint(const CSubSpace &h2, const char*) const
{
    CSubSpace i = intersect(h2);
    if (i.getdim() != 0)
        throw runtime_error("CSubSpace::intersectionPoint : intersection is not a point");
    return i.getp();
}


/** Create a CSubSpace representing a 3D line from a point and a vector.
 *
 * @param p a point of the line
 * @param v a vector along the line
 */
CSubSpace CSubSpace::line(const CPoint3d &p, const CVector3d &v)
{
    if (!v.length())
    {
        cout << "CSubSpace::line : Crash point = " << p << endl;
        throw invalid_argument("CSubSpace::line : Input vector cannot be zero point ");
    }
    CMatrix m(3, 1);
    for (int i = 0; i < 3; ++i)
        m(i, 0) = v[i];
    return CSubSpace(p, m, GEOCPP_FROM_BASE);
}


/** Create a CSubSpace representing a 3D plane from a point and two vectors.
 *
 * @param p a point of the plane
 * @param v1 a vector in the plane
 * @param v2 a second vector in the plane, not colinear with v1
 */
CSubSpace CSubSpace::plane(const CPoint3d &p, const CVector3d &v1, const CVector3d& v2)
{
    if (!CVector3d::crossProduct(v1, v2).length())
    {
        cout << "CSubSpace::plane : Crash point = " << p << endl;
        throw invalid_argument("CSubSpace::plane : 2 Vectors cannot be colinear");
    }
    CMatrix m(3,2);
    for (int i = 0; i < 3; i++)
    {
        m(i, 0) = v1[i];
        m(i, 1) = v2[i];
    }
    return CSubSpace(p, m, GEOCPP_FROM_BASE);
}


#if 0
/*******************************************************

                      Other functions

********************************************************/

/** Outputs a CMatrix to a stream.
 */
ostream& operator<< (ostream &o, const CMatrix &m)
{
    o << "[";
    for (size_t i = 0 ; i < m.rows() ; i++)
    {
        o << m.row(i);
        if ( i != m.rows() - 1 )
            o << endl;
    }
    o << "]";
    return o;
}


/** Outputs the definition of a CSubSpace to a stream.
 */
ostream& operator<< (ostream &o, const CSubSpace &h)
{
    o << "--------------------------------" << endl;
    o << "     subspace : ";
    switch (h.getdim())
    {
    case -1:
        o << "empty" << endl;
        break;
    case 0:
        o << "point" << endl;
        break;
    case 1:
        o << "line" << endl;
        break;
    case 2:
        o << "plane" << endl;
        break;
    default:
        o << "dim=" << h.getdim() << endl;
    }
    o << "--------------------------------" << endl;
    if ( h.getdim() >= 0 )
    {
        o << "point:" << endl << h.getp() << endl;
        if (h.getdim()>0)
        {
            o << "------" << endl;
            o << "base vectors (in columns): " << endl << h.getm().kern(h.getp().size()) << endl << "------" << endl;
            o << "equations (coeffs in lines): " << endl << h.getm() << endl;
        }
        o << "--------------------------------" << endl;
    }
    return o;
}
#endif

