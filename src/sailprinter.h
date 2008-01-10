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

#ifndef SAILPRINTER_H
#define SAILPRINTER_H

#include "sailpainter.h"

class CPanelGroup;
class CSailDef;
class QPaintDevice;
class QPrinter;


/** A class for printing sails or sail data.
 */
class CSailPrinter : public QObject
{
public:
    CSailPrinter(QPrinter *printer, unsigned int fontsize = 10);
    CSailPrinter(QPaintDevice *pd, unsigned int fontsize = 10);

    bool newPage();
    real printHeader(const QString title);
    real printDataSection(const QString title);
    real printDataLine(const QString title, const QString data0 = "", const QString data1 = "", const QString data2 = "");
    void printSailData(const CSailDef &saildef);
    void printSailDevel(const CPanelGroup &flatsail);
    void printSailDrawing(const CPanelGroup &sail);

    void setShowLabels(bool show) { showLabels = show; };

protected:
    void init(unsigned int fontsize);

    /** painter to draw sail objects */
    CSailPainter painter;
    /** current X position when printing text */
    real xPos;
    /** current Y position when printing text */
    real yPos;
    /** is the area we are writing to a QPrinter? */
    bool isPrinter;
    /** should the labels be printed? */
    bool showLabels;
};

#endif
