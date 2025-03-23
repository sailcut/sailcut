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

#ifndef SAILPAINTER_H
#define SAILPAINTER_H

#include <QPainter>
#include <geocpp/geocpp.h>

class CSide;
class CPanel;
class CPanelLabel;
class CPanelGroup;

/** A sail painter.
 *
 * @see CSailDispLabel
 */
class CSailPainter : public QPainter
{
public:
    /** Construct a new sail painter.
     *
     * @param dev the paint device we are operating on
    */
    CSailPainter(QPaintDevice *dev) : QPainter(dev)
    {};

    void draw(const CPanel &panel);
    void draw(const CPanelGroup &sail);
    void drawArrowLabel(const CPoint3d &pDisp, const QStringList &lst, const real angle);
    void drawCoord(const CPoint3d &p, const real angle);
    void drawDelta(const CPoint3d &pDisp, const CVector3d &vValue, const real angle);
    void drawCross(const CPoint3d &p, const real size);
    void drawTextCentered(const CPoint3d &p, const QString &str);
    void drawTextCentered(const CPoint3d &p, const QStringList &lst);
    void drawLabels(const CPanel &panel);
    void drawLabels(const CPanelGroup &sail);
    void drawMarkers(const CPanel &panel);
    void drawMarkers(const CPanelGroup &sail);

    void setFontSize(const real size, const real zoom);
    void setPenColor(const QColor &color);
    void setPenWidth();
    void setWindow(const CRect3d& lRect);

    CVector3d textSize(const QStringList &lst);
    CRect3d viewRect() const;
};


/** A text painter.
 */
class CTextPainter : public CSailPainter
{
public:
    CTextPainter(QPaintDevice *pd);
    void printReset();
    void printHeader(const QString title);
    void printDataSection(const QString title);
    void printDataLine(const QString title, const QString data0 = "", const QString data1 = "", const QString data2 = "");

protected:
    /** current x offset for text output */
    real xPos;
    /** current y offset for text output */
    real yPos;
};

#endif
