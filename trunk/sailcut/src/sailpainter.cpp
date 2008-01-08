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

#include "sailpainter.h"
#include "panelgroup.h"

QPolygonF& operator<<(QPolygonF &poly, const CPoint3d &p)
{
    poly << QPointF(p.x(), - p.y());
    return poly;
}


/** Draws a panel of a sail.
 */
void CSailPainter::draw(const CPanel &panel)
{
    //setPen(Qt::blue);
    CSide::const_iterator iter;
    CSide::const_reverse_iterator riter;

    QPolygonF poly;
    for (iter = panel.bottom.begin(); iter != panel.bottom.end(); iter++)
        poly << *iter;
    for (iter = panel.right.begin(); iter != panel.right.end(); iter++)
        poly << *iter;
    for (riter = panel.top.rbegin(); riter != panel.top.rend(); riter++)
        poly << *riter;
    for (riter = panel.left.rbegin(); riter != panel.left.rend(); riter++)
        poly << *riter;
    drawPolyline(poly);

    if (panel.hasHems)
    {
        setPen(Qt::red);

        poly = QPolygonF();
        for (iter = panel.cutBottom.begin(); iter != panel.cutBottom.end(); iter++)
            poly << *iter;
        for (iter = panel.cutRight.begin(); iter != panel.cutRight.end(); iter++)
            poly << *iter;
        for (riter = panel.cutTop.rbegin(); riter != panel.cutTop.rend(); riter++)
            poly << *riter;
        for (riter = panel.cutLeft.rbegin(); riter != panel.cutLeft.rend(); riter++)
            poly << *riter;
        drawPolyline(poly);

        // reset pen color
        setPen(Qt::black);
    }
}


/** Draws a panel label.
 */
void CSailPainter::draw(const CPanelLabel &label)
{
    setPen(Qt::black);
    
    drawTextCentered(label.origin, QStringList(label.name));
}


/** Draws a complete sail.
 */
void CSailPainter::draw(const CPanelGroup &sail)
{
    unsigned int i;
    
    for( i = 0; i < sail.size(); i++ )
    {   
        if ( sail.type == HULL )
            setPen(Qt::darkGreen);
        else if ( sail.type == RIG )
            setPen(Qt::darkRed);
        else
            setPen(Qt::blue);
             
        draw(sail[i]);
    }
    for( i = 0; i < sail.child.size(); i++ )
        draw(sail.child[i]);
        
    // reset pen color 
    setPen(Qt::black);
        
}


/** Print a label with a line to a point.
 *
 * @param pDisp the display point coordinates
 * @param lst a list of lines of text to print
 * @param angle
 */
void CSailPainter::drawArrowLabel(const CPoint3d &pDisp, const QStringList &lst, const real angle)
{
    CVector3d textDim = textSize(lst);

    CVector3d v = CMatrix::rot3d(2, angle) * CVector3d(1, 0, 0);
    CPoint3d arrowEnd = pDisp + real(2 * fontMetrics().height()) * v;

    // the distance from the arrow end to the center of the text box
    real dist;
    if (fabs(v.x() * textDim.y()) < fabs(textDim.x() * v.y()))
    {
        // the arrow touches the text box on the top or bottom sides
        dist = fabs(textDim.y() / (2.0 * sin(angle)));
    }
    else
    {
        // the arrow touches the text box on the left or right sides
        dist = fabs(textDim.x() / (2.0 * cos(angle)));
    }
    CPoint3d textCenter = arrowEnd + 1.2 * dist * v;

    drawLine(int(pDisp.x()), -int(pDisp.y()), int(arrowEnd.x()), -int(arrowEnd.y()));
    drawTextCentered(textCenter, lst);
}


/** Draws a cross at a given Point position.
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
    setPen(Qt::black);
    drawTextCentered(p, QStringList(str));
}


/** Draw text centered at the given position.
 */
void CSailPainter::drawTextCentered(const CPoint3d &p, const QStringList &lst)
{
    CVector3d dim = textSize(lst);
    setPen(Qt::black);

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
void CSailPainter::drawLabels(const CPanelGroup &sail)
{
    unsigned int i;
    setPen(Qt::black);
    
    for ( i = 0; i < sail.size(); i++ )
        draw(sail[i].label);
    for ( i = 0; i < sail.child.size(); i++ )
        drawLabels(sail.child[i]);
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
    return CRect3d(CPoint3d(0,0,0), CPoint3d(viewport().width(), viewport().height()));
}


