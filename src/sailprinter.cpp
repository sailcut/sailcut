/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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

#include "sailprinter.h"

#include <QPrinter>

#include "sailcalc.h"
#include "saildef.h"
#include "panelgroup.h"
#include "disparea.h"


/** Construct a new CSailPrinter.
 *
 * @param pd
 * @param fontsize
 */
CSailPrinter::CSailPrinter(QPaintDevice *pd, unsigned int fontsize)
        : painter(pd), isPrinter(false)
{
    init(fontsize);
}

/** Construct a new CSailPrinter.
 *
 * @param printer
 * @param fontsize
 */
CSailPrinter::CSailPrinter(QPrinter *printer, unsigned int fontsize)
        : painter(printer), isPrinter(true)
{
    init(fontsize);
}

/** Initialise font size and cursor position.
 *
 * @param fontsize
 */
void CSailPrinter::init(unsigned int fontsize)
{
    painter.setFont(QFont("times", fontsize));

    // half inch margin on top and left
    xPos = painter.device()->logicalDpiX() / 2;
    yPos = painter.device()->logicalDpiY() / 2;
}

/** Print a header banner (used at the top of a for example).
 *
 * @param title the title to print
 */
real CSailPrinter::printHeader(const QString title)
{
    QFontMetrics fm(painter.font());
    QString btitle = "  " + title + "  ";
    painter.drawText(int(xPos), int(yPos), btitle);

    // draw box around header
    painter.drawRect(int(xPos), int(yPos - fm.height()), fm.width(btitle), int(1.5*fm.height()));

    yPos += 2 * fm.height();
    return yPos;
}


/** Start a new page.
 */
bool CSailPrinter::newPage()
{
    if (isPrinter)
    {
        QPrinter *printer = (QPrinter*)painter.device();
        return printer->newPage();
    }
    else
    {
        return true;
    }
}


/** Print a data section title.
 *
 * @param title the title of the section
 */
real CSailPrinter::printDataSection(const QString title)
{
    QFontMetrics fm(painter.font());
    yPos += 0.5 * fm.height();
    painter.drawText(int(xPos), int(yPos), title);

    yPos += 1.1 * fm.height();
    return yPos;
}


/** Print a line of data.
 *
 * @param title the title for the current line of data
 * @param data0 first value
 * @param data1 second value
 * @param data2 third value
 */
real CSailPrinter::printDataLine(const QString title, const QString data0, const QString data1, const QString data2)
{
    QFontMetrics fm(painter.font());

    unsigned int x1 = int(xPos + 2  * fm.width("X"));
    unsigned int x2 = int(x1   + 26 * fm.width("X"));
    unsigned int x3 = int(x2   + 13 * fm.width("X"));
    unsigned int x4 = int(x3   + 13 * fm.width("X"));

    painter.drawText(x1, int(yPos), title);
    painter.drawText(x2, int(yPos), data0);
    painter.drawText(x3, int(yPos), data1);
    painter.drawText(x4, int(yPos), data2);

    yPos += 1.1 * fm.height();
    return yPos;
}


/**
 * Print the current sail data.
 *
 * @param saildef
 */
void CSailPrinter::printSailData(const CSailDef &saildef)
{
    QString text2=" ", text3=" ";

    // text of page header
    printHeader(tr("Sailcut CAD data sheet") + (( saildef.sailID.length() > 0 ) ? " - " + saildef.sailID : " "));

    // sail cut and type
    switch (saildef.sailType )
    {
    case MAINSAIL:
        text2 = tr("Mainsail");
        break;
    case JIB:
        text2 = tr("Jib");
        break;
    case WING:
        text2 = tr("Wing")+" @ " + QString::number(saildef.dihedralDeg) + tr("deg");
        break;
    }
    printDataLine(tr("Sail type"), text2, " ");

    text3 = " ";
    switch ( saildef.sailCut )
    {
    case CROSS:
        text2 = tr("Cross Cut");
        break;
    case HORIZONTAL:
        text2 = tr("Horizontal Cut");
        break;
    case RADIAL:
        text2 = tr("Radial Cut");
        text3  = QString::number(saildef.nbSections) + " " + tr("sections") + ",   ";
        text3  += QString::number(saildef.nbGores) + " " + tr("head gores") + ",   ";
        text3  += QString::number(saildef.nbLuffGores) + " " + tr("luff gores") + ".";
        break;
    case TWIST:
        text2 = tr("Twist Foot Cut");
        break;
    case VERTICAL:
        text2 = tr("Vertical Cut");
        break;
    case MITRE:
        text2 = tr("Mitre Cut");
        break;
    }
    printDataLine(tr("Sail layout"), text2, text3);

    // boat data
    printDataSection(tr("Rig"));
    printDataLine(tr("Boat LOA"), QString::number(saildef.LOA), "mm");
    printDataLine(tr("Mast/Luff rake"), QString::number(saildef.rake), "mm");
    printDataLine(tr("Tack position X"), QString::number(saildef.tackX), "mm");
    printDataLine(tr("Tack height Y"), QString::number(saildef.tackY), "mm");
    printDataLine(tr("Fore triangle hoist I"), QString::number(saildef.foreI), "mm");
    printDataLine(tr("Fore triangle base J"), QString::number(saildef.foreJ), "mm");

    // sides of the sail
    printDataSection(tr("Sail dimensions"));
    printDataLine(tr("Luff length"), QString::number(saildef.luffL), "mm");
    printDataLine(tr("Foot length"), QString::number(saildef.footL), "mm");
    printDataLine(tr("Leech length"), QString::number(saildef.leechL), "mm");
    printDataLine(tr("Gaff length"), QString::number(saildef.gaffL), "mm");
    printDataLine(tr("Gaff angle wrt luff"), QString::number(saildef.gaffDeg), "deg");

    // shape of sides
    printDataSection(tr("Shape of edges"));
    printDataLine(tr("Luff round"), QString::number(saildef.luffR), "mm");
    printDataLine(tr("Luff round position"), QString::number(saildef.footRP), "%");
    printDataLine(tr("Foot round"), QString::number(saildef.footR), "mm");
    printDataLine(tr("Foot round position"), QString::number(saildef.footRP), "%");
    printDataLine(tr("Leech round"), QString::number(saildef.leechR), "mm");
    printDataLine(tr("Leech round position"), QString::number(saildef.leechRP), "%");
    printDataLine(tr("Gaff round"), QString::number(saildef.gaffR), "mm");
    printDataLine(tr("Gaff round position"), QString::number(saildef.gaffRP), "%");

    // sail setting
    printDataSection(tr("Sail settings"));
    printDataLine(tr("Twist angle"), QString::number(saildef.twistDeg), "deg");
    printDataLine(tr("Sheeting angle"), QString::number(saildef.sheetDeg), "deg");

    // cloth width, seam and hems width
    printDataSection(tr("Cloth seams and hems"));
    printDataLine(tr("Cloth width"), QString::number(saildef.clothW), "mm");
    printDataLine(tr("Seams width"), QString::number(saildef.seamW), "mm");
    printDataLine(tr("Leech hem width"), QString::number(saildef.leechHemW), "mm");
    printDataLine(tr("Other hem width"), QString::number(saildef.hemsW), "mm");

    // sail mould
    printDataSection(tr("Sail mould"));
    printDataLine("", tr("Luff factor"), tr("Depth"), tr("Leech factor"));
    printDataLine(
        tr("Top profile"),
        QString::number( saildef.mould.profile[2].getLuff() ),
        QString::number( saildef.mould.profile[2].getDepth()*100 )+ "%",
        QString::number( saildef.mould.profile[2].getLeech()*50));

    printDataLine(
        tr("Mid profile at h = ") + QString::number( saildef.mould.vertpos ) + "%",
        QString::number( saildef.mould.profile[1].getLuff() ),
        QString::number( saildef.mould.profile[1].getDepth()*100 )+"%",
        QString::number( saildef.mould.profile[1].getLeech()*50));

    printDataLine(
        tr("Bottom profile"),
        QString::number( saildef.mould.profile[0].getLuff() ),
        QString::number( saildef.mould.profile[0].getDepth()*100 )+"%",
        QString::number( saildef.mould.profile[0].getLeech()*50));
}


/** Print a label with a line to a point.
 *
 * @param pDisp the display point coordinates
 * @param lst a list of lines of text to print
 * @param angle
 */
void CSailPrinter::printArrowLabel(const CPoint3d &pDisp, const QStringList &lst, const real angle)
{
    CVector3d textDim = painter.textSize(lst);

    CVector3d v = CMatrix::rot3d(2, angle) * CVector3d(1, 0, 0);
    CPoint3d arrowEnd = pDisp + real(2 * painter.fontMetrics().height()) * v;

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

    painter.drawLine(int(pDisp.x()), -int(pDisp.y()), int(arrowEnd.x()), -int(arrowEnd.y()));
    painter.drawTextCentered(textCenter, lst);
}


/** Print a point's coordinates.
 *
 * @param pDisp the display point coordinates
 * @param pValue the real coordinates
 * @param angle
 */
void CSailPrinter::printCoord(const CPoint3d &pDisp, const CPoint3d &pValue, const real angle)
{
    // build list of lines to print
    QStringList lst;
    lst.append(QString("X=") + QString::number(pValue.x(), 'f', 1));
    lst.append(QString("Y=") + QString::number(pValue.y(), 'f', 1));
    printArrowLabel(pDisp, lst, angle);
}


/** Print a delta.
 *
 * @param pDisp the display point coordinates
 * @param vValue the distances
 * @param angle
 */
void CSailPrinter::printDelta(const CPoint3d &pDisp, const CVector3d &vValue, real angle)
{
    // build list of lines to print
    QStringList lst;
    lst.append(QString("dX=") + QString::number(vValue.x(), 'f', 1));
    lst.append(QString("dY=") + QString::number(vValue.y(), 'f', 1));
    printArrowLabel(pDisp, lst, angle);
}


/** Print a developed sail.
 *
 * @param flatsail
 */
void CSailPrinter::printSailDevel(const CPanelGroup &flatsail)
{
    unsigned int npt;

    real dx=0, dy=0;
    
    // center the sail
    CPanelGroup printSail = flatsail + CVector3d( -flatsail.boundingRect().center() );

    // calculate logical rectangle
    real zoom = 0.8;
    CDispArea disparea(zoom);
    CRect3d logicalRect = disparea.calcLRect(painter.viewRect(), printSail.boundingRect());

    // print the panels out one by one
    for (unsigned int i = 0; i < flatsail.panel.size(); i++)
    {
        if ( i > 0 )
        {
            newPage();
        }
        // set coordinate system to match the logical viewport
        painter.setWindow(logicalRect);
        painter.setFontSize(10, zoom);

        CPanel currentPanel = printSail.panel[i];
        CRect3d rp = currentPanel.boundingRect();
        painter.draw(currentPanel);

        // mark corners of cloth rectangle
        painter.setPen(Qt::green);
        painter.drawCross(rp.min, painter.fontMetrics().height() );
        printCoord(rp.min, CPoint3d(0,0,0), PI );

        // print the current panel number
        painter.drawTextCentered(currentPanel.centroid(), currentPanel.label.name);
        painter.setPen(Qt::blue);

        /* add coordinates of inner lines */
        // top fwd corners
        npt = 0;
        printCoord(currentPanel.top.point[npt], flatsail.panel[i].top.point[npt], 2*PI/3 );

        // top  middle
        npt = int ( (currentPanel.top.nbpoints() -1) /2 );
        if ( CVector3d(flatsail.panel[i].top.point[npt]-flatsail.panel[i].top.point[0]).norm() > 5 )
        {
            dx = CVector3d( flatsail.panel[i].top.point[npt] - flatsail.panel[i].top.point[0] ) * CVector3d( flatsail.panel[i].top.point[flatsail.panel[i].top.nbpoints() -1] - flatsail.panel[i].top.point[0] ).unit();
            dy = Distance3d(flatsail.panel[i].top.point[npt] , flatsail.panel[i].top.point[0] , flatsail.panel[i].top.point[flatsail.panel[i].top.nbpoints()-1] );
            printDelta(currentPanel.top.point[npt], CVector3d(dx, dy, 0), PI/2);
        }

        // top aft corner
        npt = currentPanel.top.nbpoints() -1;
        if ( CVector3d(flatsail.panel[i].top.point[npt]-flatsail.panel[i].top.point[0]).norm() > 5 )
        {
            printCoord(currentPanel.top.point[npt], flatsail.panel[i].top.point[npt], PI/3);
        }

        // right middle
        npt = (currentPanel.right.nbpoints() -1)/2;
        printCoord(currentPanel.right.point[npt], flatsail.panel[i].right.point[npt], 0);

        // bottom left corner
        if ( CVector3d(flatsail.panel[i].top.point[0]-flatsail.panel[i].bottom.point[0]).norm() > 5 )
        {
            npt = 0;
            printCoord(currentPanel.bottom.point[npt], flatsail.panel[i].bottom.point[npt], -2*PI/3 );

            // mid left
            npt = (currentPanel.left.nbpoints() -1)/2;
            printCoord(currentPanel.left.point[npt], flatsail.panel[i].left.point[npt], PI );
            if ( i == flatsail.panel.size() )
            {
                npt = 1+(currentPanel.left.nbpoints() -1)/2;
                printCoord(currentPanel.left.point[npt], flatsail.panel[i].left.point[npt], 2*PI/3 );
            }
        }

        // bottom intermediate fwd
        npt = int ( (currentPanel.bottom.nbpoints() -1) /5 );
        dx = CVector3d(  flatsail.panel[i].bottom.point[npt] -  flatsail.panel[i].bottom.point[0] ) * CVector3d(  flatsail.panel[i].bottom.point[ flatsail.panel[i].bottom.nbpoints() -1] -  flatsail.panel[i].bottom.point[0] ).unit();
        dy = Distance3d( flatsail.panel[i].bottom.point[npt] ,  flatsail.panel[i].bottom.point[0] ,  flatsail.panel[i].bottom.point[ flatsail.panel[i].bottom.nbpoints()-1] );
        printDelta(currentPanel.bottom.point[npt], CVector3d(dx, dy, 0), -5*PI/8 );

        // bottom intermediate middle
        npt = int ( (currentPanel.bottom.nbpoints() -1) /2 );
        dx = CVector3d(  flatsail.panel[i].bottom.point[npt] -  flatsail.panel[i].bottom.point[0] ) * CVector3d(  flatsail.panel[i].bottom.point[ flatsail.panel[i].bottom.nbpoints() -1] -  flatsail.panel[i].bottom.point[0] ).unit();
        dy = Distance3d( flatsail.panel[i].bottom.point[npt] ,  flatsail.panel[i].bottom.point[0] ,  flatsail.panel[i].bottom.point[ flatsail.panel[i].bottom.nbpoints()-1] );
        printDelta(currentPanel.bottom.point[npt], CVector3d(dx, dy, 0), -PI/2 );

        // bottom intermediate aft
        npt = int ( (currentPanel.bottom.nbpoints() -1) *4/5 );
        dx = CVector3d(  flatsail.panel[i].bottom.point[npt] -  flatsail.panel[i].bottom.point[0] ) * CVector3d(  flatsail.panel[i].bottom.point[ flatsail.panel[i].bottom.nbpoints() -1] -  flatsail.panel[i].bottom.point[0] ).unit();
        dy = Distance3d( flatsail.panel[i].bottom.point[npt] ,  flatsail.panel[i].bottom.point[0] ,  flatsail.panel[i].bottom.point[ flatsail.panel[i].bottom.nbpoints()-1] );
        printDelta(currentPanel.bottom.point[npt], CVector3d(dx, dy, 0), -3*PI/8 );

        // bottom aft corner
        npt = currentPanel.bottom.nbpoints() -1;
        printCoord(currentPanel.bottom.point[npt], flatsail.panel[i].bottom.point[npt], -PI/3 );

        // reset pen color
        painter.setPen(Qt::black);

    }

}


/** Print the drawing of a sail.
 *
 * @param sail
 */
void CSailPrinter::printSailDrawing(const CPanelGroup &sail)
{
    // center the sail
    CPanelGroup printSail = sail + CVector3d( -sail.boundingRect().center() );

    // calculate logical rectangle
    real zoom = 0.8;
    CDispArea disparea(zoom);
    CRect3d logicalRect = disparea.calcLRect(painter.viewRect(), printSail.boundingRect());
    
    // set coordinate system to match the logical viewport
    painter.setWindow(logicalRect);
    painter.setFontSize(10, zoom);

    painter.draw(printSail);
    painter.drawLabels(printSail);
}
