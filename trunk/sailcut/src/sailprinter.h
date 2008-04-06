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

#ifndef SAILPRINTER_H
#define SAILPRINTER_H

#include "saildef.h"
#include "panelgroup.h"
#include "sailpainter.h"
#include "printer.h"

class QPaintDevice;
class QPrinter;

/** A class for printing sail data.
 *
 *  All the data is printed on a single page.
 */
class CSailDataPrinter : public CPrinter
{
public:
    CSailDataPrinter(const CSailDef &obj) : saildef(obj) {}; 
    size_t pages() const { return 1; };
    void print(CTextPainter *painter, size_t page) const;

protected:
    /** the sail definition to print */
    const CSailDef saildef;
};


/** A class for printing developped sail panels.
 *
 *  One panel is output per page.
 */
class CSailDevelPrinter : public CPrinter
{
public:
    CSailDevelPrinter(const CPanelGroup &obj, bool show_labels = true)
        : flatsail(obj), showLabels(show_labels) {};
    size_t pages() const { return flatsail.size(); };
    void print(CTextPainter *painter, size_t page) const;

protected:
    /** the flat sail to print */
    const CPanelGroup flatsail;
    /** should the labels be printed? */
    bool showLabels;
};


/** A class for printing a sail.
 *
 *  All the panels are printed on a single page.
 */
class CSailDrawingPrinter : public CPrinter
{
public:
    CSailDrawingPrinter(const CPanelGroup &obj, bool show_labels = true)
        : sail(obj), showLabels(show_labels) {};
    size_t pages() const { return 1; };
    void print(CTextPainter *painter, size_t page) const;

protected:
    /** the sail to print */
    const CPanelGroup sail;
    /** should the labels be printed? */
    bool showLabels;
};


#endif
