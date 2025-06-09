/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
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

#include <geocpp/geocpp.h>

#include <cstdlib>
#include <ctime>

#ifdef DEBUG
#include <mcheck.h>
#endif

/** Outputs a CMatrix to a stream.
 */
std::ostream& operator<< (std::ostream &o, const CMatrix &m)
{
    o << "[";
    for (size_t i = 0 ; i < m.rows() ; ++i)
    {
        for (size_t j = 0; j < m.columns(); ++j)
            o << m(i, j);
        if ( i != m.rows() - 1 )
            o << std::endl;
    }
    o << "]";
    return o;
}

class tst_GeoCpp : public QObject
{
    Q_OBJECT

private slots:
    void testVector();
    void testRect();
    void testSubSpace();
};


void tst_GeoCpp::testVector()
{
    CVector3d u3(1, 2, 3);
    CVector3d v3(5, 8, 9);
    QCOMPARE(u3, CVector3d(1, 2, 3));
    //QCOMPARE(u3.length(), 3.74166);
    QCOMPARE(u3.x(), 1.0);
    QCOMPARE(u3.y(), 2.0);
    QCOMPARE(u3.z(), 3.0);

    u3.setX(5);
    QCOMPARE(u3, CVector3d(5, 2, 3));
    QCOMPARE(u3.x(), 5.0);
    QCOMPARE(u3.y(), 2.0);
    QCOMPARE(u3.z(), 3.0);

    QCOMPARE(v3, CVector3d(5, 8, 9));
    QCOMPARE(-v3, CVector3d(-5, -8, -9));
    QCOMPARE(u3 + v3, CVector3d(10, 10, 12));
    QCOMPARE(u3 - v3, CVector3d(0, -6, -6));
    QCOMPARE(u3 * 2, CVector3d(10, 4, 6));
    QCOMPARE(2 * u3, CVector3d(10, 4, 6));

    u3[2] = 456;
    QCOMPARE(u3, CVector3d(5, 2, 456));
    QCOMPARE(u3.x(), 5.0);
    QCOMPARE(u3.y(), 2.0);
    QCOMPARE(u3.z(), 456.0);
}


void tst_GeoCpp::testRect()
{
    CRect3d r(CPoint3d(-2, -2, -2), CPoint3d(1, 1, 1));
    QCOMPARE(r.min, CPoint3d(-2, -2, -2));
    QCOMPARE(r.max, CPoint3d(1, 1, 1));
    QCOMPARE(r.center(), CPoint3d(-0.5, -0.5, -0.5));
    QCOMPARE(r.height(), 3.0);
    QCOMPARE(r.width(), 3.0);

    CRect3d r2 = r * 3;
    QCOMPARE(r2.min, CPoint3d(-5, -5, -5));
    QCOMPARE(r2.max, CPoint3d(4, 4, 4));
    QCOMPARE(r2.center(), CPoint3d(-0.5, -0.5, -0.5));
    QCOMPARE(r2.height(), 9.0);
    QCOMPARE(r2.width(), 9.0);
}


void tst_GeoCpp::testSubSpace(void)
{
    const CSubSpace L3A = CSubSpace::line(CPoint3d(0,0,1), CVector3d(1,1,0));
    const CSubSpace L3B = CSubSpace::line(CPoint3d(0,0,0), CVector3d(1,1,1));
    const CSubSpace P3A = CSubSpace::plane(CPoint3d(2,3,7), CVector3d(1,0,5), CVector3d(0,1,0));
    const CSubSpace P3B = CSubSpace::plane(CPoint3d(4,-5,1), CVector3d(-3,0,1), CVector3d(0,1,0));
    const CPoint3d p3(2,2,2),q3(2,1,2),r3(4,5,7),s3(2,2,1);

    CSubSpace h;

    // lines
    QCOMPARE(L3A.contains(p3), false);
    QCOMPARE(L3A.contains(s3), true);

    QCOMPARE(L3B.contains(p3), true);
    QCOMPARE(L3B.contains(q3), false);

    // planes
    QCOMPARE(P3A.contains(p3), false);
    QCOMPARE(P3A.contains(r3), false);

    QCOMPARE(P3B.contains(p3), false);
    QCOMPARE(P3B.contains(r3), false);

    // line / line intersection
    h = L3A.intersect(L3B);
    QCOMPARE(h.getdim(), 0);
    QCOMPARE(L3A.contains(h.getp()), true);
    QCOMPARE(L3B.contains(h.getp()), true);

    // plane / plane intersection
    h = P3A.intersect(P3B);
    QCOMPARE(h.getdim(), 1);
    QCOMPARE(P3A.contains(h.getp()), true);
    QCOMPARE(P3B.contains(h.getp()), true);

    // plane / line intersection
    h = P3A.intersect(L3A);
    QCOMPARE(h.getdim(), 0);
    QCOMPARE(P3A.contains(h.getp()), true);
    QCOMPARE(L3A.contains(h.getp()), true);

    // line / plane intersection
    h = L3A.intersect(P3A);
    QCOMPARE(h.getdim(), 0);
    QCOMPARE(P3A.contains(h.getp()), true);
    QCOMPARE(L3A.contains(h.getp()), true);
}

QTEST_MAIN(tst_GeoCpp)
#include "tst_geocpp.moc"
