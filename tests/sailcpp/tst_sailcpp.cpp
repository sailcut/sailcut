/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
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

#include <cstdlib>
#include <ctime>

#include <QObject>
#include <QtTest>

#include "geocpp/geocpp.h"
#include "sailcpp/panel.h"

class tst_SailCpp : public QObject
{
    Q_OBJECT

private slots:
    void testSide();
};

void tst_SailCpp::testSide()
{
    CSide side(0);

    // Empty.
    QCOMPARE(side.left(), 0);
    QCOMPARE(side.bottom(), 0);

    // 1 point.
    side.push_back(CPoint3d(1, 2, 3));
    QCOMPARE(side.left(), 1);
    QCOMPARE(side.bottom(), 2);

    // 2 points
    side.push_back(CPoint3d(2, 3, 4));
    QCOMPARE(side.left(), 1);
    QCOMPARE(side.bottom(), 2);

    // 2 points
    side.push_back(CPoint3d(-1, -2, -3));
    QCOMPARE(side.left(), -1);
    QCOMPARE(side.bottom(), -2);
}

QTEST_MAIN(tst_SailCpp)
#include "tst_sailcpp.moc"
