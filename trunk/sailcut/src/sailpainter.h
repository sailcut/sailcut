/*
 * Copyright (C) 1993-2008 Robert & Jeremy Laine
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SAILPAINTER_H
#define SAILPAINTER_H

#include <QPainter>
#include <geocpp/geocpp.h>

class CSide;
class CPanel;
class CPanelLabel;
class CPanelGroup;

/** A sail painter
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
    void draw(const CPanelLabel &label);
    void drawArrowLabel(const CPoint3d &pDisp, const QStringList &lst, const real angle);
    void drawCross(const CPoint3d &p, const real size);
    void drawTextCentered(const CPoint3d &p, const QString &str);
    void drawTextCentered(const CPoint3d &p, const QStringList &lst);
    void drawLabels(const CPanelGroup &sail);

    void setFontSize(const real size, const real zoom);
    void setWindow(const CRect3d& lRect);

    CVector3d textSize(const QStringList &lst);
    CRect3d viewRect() const;
};

#endif
