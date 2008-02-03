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

#include "sailprinter.h"

#include <QPrinter>

#include "sailcalc.h"
#include "saildef.h"
#include "panelgroup.h"
#include "sailcalc.h"


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
    showLabels = true;

    // half inch margin on left quarter inch on top
    xPos = painter.device()->logicalDpiX() / 2;
    yPos = painter.device()->logicalDpiY() / 4;
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

    yPos += 1.5 * fm.height();
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

    yPos += 1 * fm.height();
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

    yPos += .8 * fm.height();
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
    QString sailID = QString::fromStdString(saildef.sailID);
    printHeader(tr("Sailcut CAD data sheet") + (( sailID.length() > 0 ) ? " - " + sailID : " "));

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


/** Print a developed sail.
 *
 * @param flatsail
 */
void CSailPrinter::printSailDevel(const CPanelGroup &flatsail)
{
    // calculate logical rectangle
    real zoom = 0.8;
    CRect3d flatrect = flatsail.boundingRect();
    CRect3d logicalRect = calcLRect(painter.viewRect(), flatrect, flatrect.center(), zoom);

    // print the panels out one by one
    for (unsigned int i = 0; i < flatsail.size(); i++)
    {
        if ( i > 0 )
        {
            newPage();
        }
        // set coordinate system to match the logical viewport
        painter.setWindow(logicalRect);
        painter.setFontSize(10, zoom);

        painter.draw(flatsail[i]);
        if (showLabels)
            painter.drawLabels(flatsail[i]);
        
        painter.drawMarkers(flatsail[i], i == (flatsail.size() - 1));

        // mark corners of cloth rectangle
        QPen oldpen = painter.pen();
        painter.setPen(Qt::green);
        CRect3d rp = flatsail[i].boundingRect();
        painter.drawCross(rp.min, painter.fontMetrics().height() );
        painter.drawCoord(rp.min, PI );
        painter.setPen(oldpen);
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
    CRect3d logicalRect = calcLRect(painter.viewRect(), printSail.boundingRect(), CPoint3d(0,0,0), zoom);

    // set coordinate system to match the logical viewport
    painter.setWindow(logicalRect);
    painter.setFontSize(10, zoom);

    painter.draw(printSail);
    if (showLabels)
        painter.drawLabels(printSail);
}
