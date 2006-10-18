/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
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

#include "sailpainter.h"
#include "sail.h"

/** Draws a side of a sail.
 */
void CSailPainter::draw(const CSide &side)
{
    QPoint prev, next;

    if (side.nbpoints() < 2)
        return;

    prev = QPoint( int(side.point[0].x()), -int(side.point[0].y()) );
    for( unsigned i = 1; i < side.nbpoints(); i++)
    {
        next = QPoint( int(side.point[i].x()), -int(side.point[i].y()) );
        drawLine(prev, next);
        prev = next;
    }
}


/** Draws a panel of a sail.
 */
void CSailPainter::draw(const CPanel &panel)
{
    setPen(Qt::blue);
    draw(panel.left);
    draw(panel.right);
    draw(panel.top);
    draw(panel.bottom);

    if (panel.hasHems)
    {
        setPen(Qt::red);
        draw(panel.cutLeft);
        draw(panel.cutRight);
        draw(panel.cutTop);
        draw(panel.cutBottom);
    }
    setPen(Qt::black);
}


/** Draws a panel label.
 */
void CSailPainter::draw(const CPanelLabel &label)
{
    drawTextCentered(label.origin, QStringList(label.name));
}


/** Draws a complete sail.
 */
void CSailPainter::draw(const CSail &sail)
{
    for( unsigned int i = 0; i < sail.nbpanels(); i++ )
        draw(sail.panel[i]);
}


/** Draws a complete sail.
 */
void CSailPainter::drawCross(const CPoint3d &p, const real size)
{
    drawLine(int(p.x()), -int(p.y()+ .5 * size), int(p.x()), -int(p.y() -.5 * size));
    drawLine(int(p.x() + .5 * size), -int(p.y()), int(p.x() - .5 * size), -int(p.y()));
}


/** Draw text centered at the given position.
 */
void CSailPainter::drawTextCentered(const CPoint3d &p, const QString &str)
{
    drawTextCentered(p, QStringList(str));
}


/** Draw text centered at the given position.
 */
void CSailPainter::drawTextCentered(const CPoint3d &p, const QStringList &lst)
{
    CVector3d dim = textSize(lst);

    int i;
    real xPos = p.x() - 0.5 * dim.x();
    real yPos = -p.y() + 0.75 * fontMetrics().height() - 0.5 * dim.y();

    for (i = 0; i < lst.size(); i++)
    {
        drawText(int(xPos), int(yPos), lst.at(i));
        yPos +=  fontMetrics().height();
    }
}


/** Draw all the sail's panel labels.
 */
void CSailPainter::drawLabels(const CSail &sail)
{
    for ( unsigned int i = 0; i < sail.nbpanels(); i++ )
        draw(sail.panel[i].label);
}


/** Set the font size.
 *
 * @param size
 * @param zoom
 */
void CSailPainter::setFontSize(const real size, const real zoom)
{
    // set the font size for labeling function of zoom
    QFont myfont = font();
    myfont.setPointSizeF(real(zoom * size * window().height()) / device()->height());
    setFont(myfont);
}


/** Set coordinate system to match the logical viewport.
 *
 * @param lRect the logical viewport rectangle
 */
void CSailPainter::setWindow(const CRect3d& lRect)
{
    QPainter::setWindow( int(lRect.min.x()),
                         int(-lRect.max.y()),
                         int(lRect.width()),
                         int(lRect.height())
                       );
}


/** Draw text centered at the given position.
 */
CVector3d CSailPainter::textSize(const QStringList &lst)
{
    CVector3d v(0, 0, 0);
    int i;

    // calculate text height and width by scanning the list of lines
    for (i = 0; i < lst.size(); i++)
    {
        real lineWidth = fontMetrics().width(lst.at(i));
        if (lineWidth > v.x())
            v.x() = lineWidth;
        v.y() += fontMetrics().height();
    }

    return v;
}


/** Return the viewport rectangle.
 */
CRect3d CSailPainter::viewRect() const
{
    CRect3d rect;
    rect.min = CPoint3d(0,0,0);
    rect.max = CPoint3d(viewport().width(), viewport().height());
    return rect;
}


