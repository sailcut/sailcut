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

#include <QObject>
#include <QtTest>

#include <geocpp/geometry.h>
#include <geocpp/subspace.h>

#include <cstdlib>
#include <ctime>

#ifdef DEBUG
#include <mcheck.h>
#endif

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

template<class T> void test_contain(const char *name, const CSubSpace &s, const T &p, bool expected)
{
    bool res = s.contains(p);
    cout << "does " << name << " contain (" << p << ") : " << s.contains(p) << endl;
    if (res != expected)
        throw runtime_error("test failed");
}


class tst_GeoCpp : public QObject
{
    Q_OBJECT

private slots:
    void testPoint();
};


void tst_GeoCpp::testPoint(void)
{
    CPoint3d p3(1,0,0), q3(0,1,0);
    CPoint3d r3;

    QCOMPARE(p3, CPoint3d(1, 0, 0));
    QCOMPARE(q3, CPoint3d(0, 1, 0));
    QCOMPARE(r3, CPoint3d(0, 0, 0));

    QCOMPARE(p3 - q3, CPoint3d(1, -1, 0));
    cout << "p3 - q3\t " << p3-q3 << endl;
    cout << "|p3,q3|\t " << CVector3d(q3 - p3).length() << endl;
    cout << " " << endl;
}


void test_rect(void)
{
    CRect3d r(CPoint3d(-2, -2, -2), CPoint3d(1, 1, 1));
    cout << "r.min\t " << r.min << endl;
    cout << "r.max\t " << r.max << endl;
    cout << "r.center()\t" << r.center() << endl;
    cout << "r2 = r * 3" << endl;
    CRect3d r2 = r * 3;
    cout << "r2.min\t " << r2.min << endl;
    cout << "r2.max\t " << r2.max << endl;
    cout << "r2.center()\t" << r2.center() << endl;
    if (r.center() != r2.center())
    {
        cout << "ERROR : r.center() != r2.center()" << endl;
    }
}


void test_vect(void)
{
    cout << "----- Vector operations -----" << endl;
    CVector3d u3(1,2,3),v3(5,8,9);
    cout << "u3\t " << u3 << endl;
    cout << "u3.x()\t " << u3.x() << endl;
    u3.setX(5);
    cout << "u3\t " << u3 << endl;
    cout << "v3\t " << v3 << endl;
    cout << "-v3\t " << -v3 << endl;
    cout << "u3 + v3\t " << u3+v3 << endl;
    cout << "u3 - v3\t " << u3-v3 << endl;
    cout << "u3 * 2\t " << u3*real(2) << endl;
    cout << "2 * u3\t " << real(2)*u3 << endl;
    u3[2] = 456;
    cout << "u3[2]\t " << u3[2] << endl;
    //cout << "u3*v3\t " << u3*v3 << endl;
    cout << " " << endl;
}

void test_matrix(void)
{
    CVector3d v1(1,2,1),v2(4,7,9);

    cout << "----- Matrix operations -----" << endl;

    // 3x2 matrix
    CMatrix q(3,2);
    q(0,0) = 2;
    q(0,1) = 4;
    q(1,0) = 6;
    q(1,1) = 3;
    q(2,0) = 4;
    q(2,1) = -1;

    cout << "q" << endl << q << endl;

#if 0
    CSubSpace h = q.solve(v1);
    cout << "q.solve(v1)" << endl << h << endl;
    if (h.getdim() != 0)
        throw runtime_error("intersection is not a point");
    cout << "q * q.solve(v1).getp()" << endl << q * h.getp() << endl;
#endif

    // Singular 3x3 matrix
    CMatrix s(3,3);
    s(0,0) = 1;
    s(0,1) = 2;
    s(0,2) = 3;
    s(1,0) = 9;
    s(1,1) = 3;
    s(1,2) = 5;
    s(2,0) = 8;
    s(2,1) = 1;
    s(2,2) = 2;

    cout << "s" << endl << s << endl;
    //cout << "s.diag()" << endl << s.diag() << endl;
    //cout << "s.img()" << endl << s.img() << endl;
    cout << "s.kern(3)" << endl << s.kern(3) << endl;
    cout << "s*s.kern(3)" << endl << s*s.kern(3) << endl;
    cout << "s*v1" << endl << s*v1 << endl;

#if 0
    CSubSpace h2 = s.solve(v1);
    cout << "s.solve(v1)" << endl << h2 << endl;
    if (h2.getdim() != 1)
        throw runtime_error("intersection is not a line");

    CSubSpace h3 = s.solve(v2);
    cout << "s.solve(v2)" << endl << h3 << endl;
    if (h3.getdim() != -1)
        throw runtime_error("intersection is not empty");
#endif

    // 3x4 matrix
    CMatrix t(3,4);
    t(0,0) = 1;
    t(0,1) = -1;
    t(0,2) = 3;
    t(0,3) = -7;
    t(1,0) = 9;
    t(1,1) = 3;
    t(1,2) = 5;
    t(1,3) = 0;
    t(2,0) = 8;
    t(2,1) = 4;
    t(2,2) = 2;
    t(2,3) = 7;

    cout << "t" << endl << t << endl;
    //cout << "t.diag()" << endl << t.diag() << endl;
    //cout << "t.img()" << endl << t.img() << endl;
    cout << "t.kern(4)" << endl << t.kern(4) << endl;
    //cout << "t.diag()*t.kern()" << endl << t.diag()*t.kern() << endl;
    cout << "t*t.kern(4)" << endl << t*t.kern(4) << endl;

    cout << " " << endl;
}

void test_space3d(void)
{
    cout << "----- 3D Subspace operations -----" << endl;
    const CSubSpace L3A = CSubSpace3d::line(CPoint3d(0,0,1), CVector3d(1,1,0));
    const CSubSpace L3B = CSubSpace3d::line(CPoint3d(0,0,0), CVector3d(1,1,1));
    const CSubSpace P3A = CSubSpace3d::plane(CPoint3d(2,3,7), CVector3d(1,0,5), CVector3d(0,1,0));
    const CSubSpace P3B = CSubSpace3d::plane(CPoint3d(4,-5,1), CVector3d(-3,0,1), CVector3d(0,1,0));
    const CPoint3d p3(2,2,2),q3(2,1,2),r3(4,5,7),s3(2,2,1);

    CSubSpace h;

    cout << "== L3A ==" << endl << L3A << endl;
    test_contain<CPoint3d>("L3A", L3A, p3, false);
    test_contain<CPoint3d>("L3A", L3A, s3, true);
    cout << endl;

    cout << "== L3B ==" << endl << L3B << endl;
    test_contain<CPoint3d>("L3B", L3B, p3, true);
    test_contain<CPoint3d>("L3B", L3B, q3, false);
    cout << endl;

    cout << "== L3A.intersect(L3B) ==" << endl << (h=L3A.intersect(L3B)) << endl;
    if (h.getdim() != 0)
        throw runtime_error("intersection is not a point");
    test_contain<CPoint3d>("L3A", L3A, h.getp(), true);
    test_contain<CPoint3d>("L3B", L3B, h.getp(), true);
    cout << endl;

    cout << "== P3A ==" << endl << P3A << endl;
    test_contain<CPoint3d>("P3A", P3A, p3, false);
    test_contain<CPoint3d>("P3A", P3A, r3, false);
    cout << endl;

    cout << "== P3B ==" << endl << P3B << endl;
    test_contain<CPoint3d>("P3B", P3B, p3, false);
    test_contain<CPoint3d>("P3B", P3B, r3, false);
    cout << endl;

    cout << "== P3A.intersect(P3B) ==" << endl << (h=P3A.intersect(P3B)) << endl;
    if (h.getdim() != 1)
        throw runtime_error("intersection is not a line");
    test_contain<CPoint3d>("P3A", P3A, h.getp(), true);
    test_contain<CPoint3d>("P3B", P3B, h.getp(), true);
    cout << endl;

    cout << "== P3A.intersect(L3A) ==" << endl << (h=P3A.intersect(L3A)) << endl;
    if (h.getdim() != 0)
        throw runtime_error("intersection is not a point");
    test_contain<CPoint3d>("P3A", P3A, h.getp(), true);
    test_contain<CPoint3d>("L3A", L3A, h.getp(), true);
    cout << endl;

    cout << "== L3A.intersect(P3A) ==" << endl << (h=L3A.intersect(P3A)) << endl;
    if (h.getdim() != 0)
        throw runtime_error("intersection is not a point");
    test_contain<CPoint3d>("P3A", P3A, h.getp(), true);
    test_contain<CPoint3d>("L3A", L3A, h.getp(), true);
    cout << endl;

    cout << " " << endl;
}

#if 0
int main()
{
#ifdef DEBUG
    mcheck(0);
#endif
    srand ( time(NULL) );

    cout << "---   Using GeoCpp library by Jeremy Laine   ---" << endl;
    cout << "(see AUTHORS file for full list of contributors)" << endl;
    cout << " " << endl;

    test_rect();
    test_vect();
    test_matrix();
    test_space3d();

    cout << "----- " << endl;

    return 0;
}
#endif

QTEST_MAIN(tst_GeoCpp)
#include "tst_geocpp.moc"
