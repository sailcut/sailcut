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

#include <cmath>

#include "sailpainter.h"
#include "sailcpp/sailcalc.h"
#include "sailcpp/panelgroup.h"

#define MIN_DISTANCE 5

QPolygonF& operator<<(QPolygonF &poly, const CPoint3d &p)
{
    poly << QPointF(p.x(), - p.y());
    return poly;
}


/** Draws a single panel of a sail.
 */
void CSailPainter::draw(const CPanel &panel)
{
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
        // switch pen to red
        const QPen oldPen = pen();
        setPenColor(Qt::red);

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
        setPen(oldPen);
    }
}


/** Draws the label of a panel.
 */
void CSailPainter::drawLabels(const CPanel &panel)
{
    drawTextCentered(panel.label.origin, QStringList(QString::fromStdString(panel.label.name)));
}


/** Draws a complete sail.
 */
void CSailPainter::draw(const CPanelGroup &sail)
{
    unsigned int i;

    // set pen color
    const QPen oldPen = pen();
    if ( sail.type == HULL )
        setPenColor(Qt::darkGreen);
    else if ( sail.type == RIG )
        setPenColor(Qt::darkRed);
    else
        setPenColor(Qt::blue);

    for (i = 0; i < sail.size(); i++)
        draw(sail[i]);
    for (i = 0; i < sail.child.size(); i++)
        draw(sail.child[i]);

    // reset pen color
    setPen(oldPen);
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
    CPoint3d arrowStart;
    CPoint3d arrowEnd;

    CVector3d v = CMatrix::rot3d(2, angle) * CVector3d(1, 0, 0);
    if (fabs(angle) > .1*PI)
        arrowEnd = pDisp + real( 2 * fontMetrics().height()) * v;
    else
        arrowEnd = pDisp + real( 1.3 * fontMetrics().height()) * v;

    CVector3d v1 = CMatrix::rot3d(2, PI/6) * v;
    arrowStart = pDisp + real(.3 * fontMetrics().height()) * v1;

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

    drawLine(int(pDisp.x()), -int(pDisp.y()), int(arrowStart.x()), -int(arrowStart.y()));
    drawLine(int(pDisp.x()), -int(pDisp.y()), int(arrowEnd.x()), -int(arrowEnd.y()));
    drawTextCentered(textCenter, lst);
}


/** Draw a label representing a point's coordinates.
 *
 * @param point
 * @param angle
 */
void CSailPainter::drawCoord(const CPoint3d &point, const real angle)
{
    // build list of lines to print
    QStringList lst;
    lst.append(QString("X=") + QString::number(point.x(), 'f', 1));
    lst.append(QString("Y=") + QString::number(point.y(), 'f', 1));
    drawArrowLabel(point, lst, angle);
}


/** Draw a label representing a distance.
 *
 * @param pDisp the display point coordinates
 * @param vValue the distances
 * @param angle
 */
void CSailPainter::drawDelta(const CPoint3d &pDisp, const CVector3d &vValue, real angle)
{
    // build list of lines to print
    QStringList lst;
    lst.append(QString("dX=") + QString::number(vValue.x(), 'f', 1));
    lst.append(QString("dY=") + QString::number(vValue.y(), 'f', 1));
    drawArrowLabel(pDisp, lst, angle);
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
void CSailPainter::drawLabels(const CPanelGroup &sail)
{
    unsigned int i;

    for (i = 0; i < sail.size(); i++)
        drawLabels(sail[i]);
    for (i = 0; i < sail.child.size(); i++)
        drawLabels(sail.child[i]);
}


/** Draw the markers for a panel.
 *
 * @param currentPanel
 */
void CSailPainter::drawMarkers(const CPanel &currentPanel)
{
    unsigned int npt = 0;
    real dx=0, dy=0;

    // top fwd corners
    drawCoord(currentPanel.top.front(), .6*PI);

    // top  middle
    npt = int ( (currentPanel.top.size() -1) /2 );
    if ( CVector3d(currentPanel.top[npt] - currentPanel.top.front()).length() > MIN_DISTANCE )
    {
        dx = CVector3d( currentPanel.top[npt] - currentPanel.top.front() ) * CVector3d( currentPanel.top.back() - currentPanel.top.front() ).unit();
        dy = Distance3d(currentPanel.top[npt] , currentPanel.top.front() , currentPanel.top.back() );
        drawDelta(currentPanel.top[npt], CVector3d(dx, dy, 0), .45*PI);
    }

    // top aft corner
    if ( CVector3d(currentPanel.top.back() - currentPanel.top.front()).length() > MIN_DISTANCE )
        drawCoord(currentPanel.top.back(), .25*PI);

    // right middle
    npt = (currentPanel.right.size() -1)/2;
    drawCoord(currentPanel.right[npt], 0.05*PI);

    // left side
    if ( CVector3d(currentPanel.left.back() - currentPanel.left.front()).length() > MIN_DISTANCE )
    {
        // left bottom
        drawCoord(currentPanel.left.front(), -.7*PI);
        // left middle
        npt = (currentPanel.left.size() -1)/2;
        drawCoord(currentPanel.left[npt], .95*PI);
        // left upper middle
        if ( CVector3d(currentPanel.left[npt+1] - currentPanel.left[npt]).length() > MIN_DISTANCE )
            drawCoord(currentPanel.left[npt+1], .8*PI);
    }

    // bottom intermediate fwd
    npt = int ( (currentPanel.bottom.size() -1) /5 );
    if ( CVector3d(currentPanel.bottom[npt] - currentPanel.bottom.front()).length() > MIN_DISTANCE )
    {
        dx = CVector3d(  currentPanel.bottom[npt] -  currentPanel.bottom.front() ) * CVector3d(  currentPanel.bottom.back() -  currentPanel.bottom.front() ).unit();
        dy = Distance3d( currentPanel.bottom[npt] ,  currentPanel.bottom.front() ,  currentPanel.bottom.back() );
        drawDelta(currentPanel.bottom[npt], CVector3d(dx, dy, 0), -.65*PI);
    }
    // bottom intermediate middle
    npt = int ( (currentPanel.bottom.size() -1) /2 );
    dx = CVector3d(  currentPanel.bottom[npt] -  currentPanel.bottom.front() ) * CVector3d(  currentPanel.bottom.back() -  currentPanel.bottom.front() ).unit();
    dy = Distance3d( currentPanel.bottom[npt] ,  currentPanel.bottom.front() ,  currentPanel.bottom.back() );
    drawDelta(currentPanel.bottom[npt], CVector3d(dx, dy, 0), -.55*PI);

    // bottom intermediate aft
    npt = int ( (currentPanel.bottom.size() -1) *4/5 );
    if ( CVector3d(currentPanel.bottom[npt] - currentPanel.bottom.back()).length() > MIN_DISTANCE )
    {
        dx = CVector3d(  currentPanel.bottom[npt] -  currentPanel.bottom.front() ) * CVector3d(  currentPanel.bottom.back() -  currentPanel.bottom.front() ).unit();
        dy = Distance3d( currentPanel.bottom[npt] ,  currentPanel.bottom.front(),  currentPanel.bottom.back() );
        drawDelta(currentPanel.bottom[npt], CVector3d(dx, dy, 0), -.4*PI );
    }
    // bottom aft corner
    drawCoord(currentPanel.bottom.back(), -.25*PI);
}


/** Draw the markers for a full sail.
 *
 * @param sail
 */
void CSailPainter::drawMarkers(const CPanelGroup &sail)
{
    for (unsigned int i = 0; i < sail.size(); i++)
        drawMarkers(sail[i]);
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

/** Set the pen color.
 *
 * Unlike setPen(QColor), this does not change the pen width.
 */
void CSailPainter::setPenColor(const QColor &color)
{
    QPen p = pen();
    p.setColor(color);
    setPen(p);
}

/** Set the default pen width.
 *
 * The width is set to 0, indicating a 1 pixel cosmetic pen.
 */
void CSailPainter::setPenWidth()
{
    QPen p = pen();
    p.setWidth(0);
    setPen(p);
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


/** Return text size vector.
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


/** Construct a new CTextPainter.
 */
CTextPainter::CTextPainter(QPaintDevice *pd)
        : CSailPainter(pd)
{
}


/** Reset the text cursors to its initial position.
 */
void CTextPainter::printReset()
{
    QFontMetrics fm(font());
    xPos = fm.width("X") * 4;
    yPos = fm.height() * 2;
}

/** Print a header banner (used at the top of a for example).
 *
 * @param title the title to print
 */
void CTextPainter::printHeader(const QString title)
{
    QFontMetrics fm(font());
    QString btitle = "  " + title + "  ";
    drawText(int(xPos), int(yPos), btitle);

    // draw box around header
    drawRect(int(xPos), int(yPos - fm.height()), fm.width(btitle), int(1.5*fm.height()));

    yPos += 1.5 * fm.height();
}


/** Print a data section title.
 *
 * @param title the title of the section
 */
void CTextPainter::printDataSection(const QString title)
{
    QFontMetrics fm(font());
    yPos += 0.5 * fm.height();
    drawText(int(xPos), int(yPos), title);

    yPos += 1 * fm.height();
}


/** Print a line of data.
 *
 * @param title the title for the current line of data
 * @param data0 first value
 * @param data1 second value
 * @param data2 third value
 */
void CTextPainter::printDataLine(const QString title, const QString data0, const QString data1, const QString data2)
{
    QFontMetrics fm(font());

    unsigned int x1 = int(xPos + 2  * fm.width("X"));
    unsigned int x2 = int(x1   + 26 * fm.width("X"));
    unsigned int x3 = int(x2   + 13 * fm.width("X"));
    unsigned int x4 = int(x3   + 13 * fm.width("X"));

    drawText(x1, int(yPos), title);
    drawText(x2, int(yPos), data0);
    drawText(x3, int(yPos), data1);
    drawText(x4, int(yPos), data2);

    yPos += .8 * fm.height();
}


