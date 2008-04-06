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

#include "panelgroup.h"
#include "sailpainter.h"
#include "printer.h"

class CSailDef;
class QPaintDevice;
class QPrinter;

/** A class for printing sail data.
 */
class CSailPrinter : public CPrinter<CSailDef>
{
public:
    CSailPrinter() {};
    size_t pages(const CSailDef &) const { return 1; };
    void print(const CSailDef &saildef, CTextPainter *painter, size_t page) const;
};


/** A class for printing developped sail panels.
 */
class CDevelPrinter : public CPrinter<CPanelGroup>
{
public:
    CDevelPrinter(bool is_printer = false, bool show_labels = true) : isPrinter(is_printer), showLabels(show_labels) {};
    size_t pages(const CPanelGroup &obj) const { return obj.size(); };
    void print(const CPanelGroup &obj, CTextPainter *painter, size_t page) const;

protected:
    /** is the area we are writing to a QPrinter? */
    bool isPrinter;
    /** should the labels be printed? */
    bool showLabels;
};


/** A class for printing a sail.
 */
class CDrawingPrinter : public CPrinter<CPanelGroup>
{
public:
    CDrawingPrinter(bool show_labels = true) : showLabels(show_labels) {};
    size_t pages(const CPanelGroup &) const { return 1; };
    void print(const CPanelGroup &obj, CTextPainter *painter, size_t page) const;

protected:
    /** should the labels be printed? */
    bool showLabels;
};


#endif
