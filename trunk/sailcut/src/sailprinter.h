/*
 * Copyright (C) 1993-2009 Robert & Jeremy Laine
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

#include "sailcpp/saildef.h"
#include "sailcpp/panelgroup.h"
#include "sailpainter.h"


/** This is the base class used for printing.
 */
class CPrinter : public QObject
{
public:
    /** Return the number of pages, must be overriden.
     */
    virtual int pages() const = 0;

    /** Perform the actual printing operation, must be overriden.
     */
    virtual void print(CTextPainter *painter, int page, real scale, real fontsize) const = 0;

    /** Return the scale needed to fit the developed sail in the given device.
    *
    * @param device
    */
    virtual double scaleToFit(QPaintDevice* device) const { return -1; };
};


/** A class for printing sail data.
 *
 *  All the data is printed on a single page.
 */
class CSailDataPrinter : public CPrinter
{
public:
    /** The constructor.
     *
     * @param obj the sail definition to print
     */
    CSailDataPrinter(const CSailDef &obj) : saildef(obj) {};
    int pages() const
    {
        return 1;
    };
    void print(CTextPainter *painter, int, real size, real fontsize) const;

protected:
    /** the sail definition to print */
    const CSailDef saildef;
};


/** A class for printing a sail.
 *
 *  All the panels are printed on a single page.
 */
class CSailDrawingPrinter : public CPrinter
{
public:
    /** The constructor.
     *
     * @param obj the sail to print
     * @param show_labels should labels be printed?
     */
    CSailDrawingPrinter(const CPanelGroup &obj, bool show_labels = true)
            : sail(obj), showLabels(show_labels) {};
    int pages() const
    {
        return 1;
    };
    void print(CTextPainter *painter, int, real scale, real fontsize) const;
    double scaleToFit(QPaintDevice *device) const;

protected:
    /** the sail to print */
    const CPanelGroup sail;
    /** should the labels be printed? */
    bool showLabels;
};


/** A class for printing developped sail panels.
 *
 *  One panel is output per page.
 */
class CSailDevelPrinter : public CSailDrawingPrinter
{
public:
    /** The constructor.
     *
     * @param obj the developed sail to print
     * @param show_labels should labels be printed?
     */
    /** The constructor.
     *
     * @param obj the sail to print
     * @param show_labels should labels be printed?
     */
    CSailDevelPrinter(const CPanelGroup &obj, bool show_labels = true)
            : CSailDrawingPrinter(obj, show_labels) {};
    int pages() const
    {
        return sail.size();
    };
    void print(CTextPainter *painter, int page, real scale, real fontsize) const;
};


#endif
