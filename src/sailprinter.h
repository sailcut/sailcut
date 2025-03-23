/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
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

#ifndef SAILPRINTER_H
#define SAILPRINTER_H

#include <QObject>

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
    virtual double scaleToFit(QPaintDevice* device) const;
};


/** A class for printing sail data.
 *
 *  All the data is printed on a single page.
 */
class CSailDataPrinter : public CPrinter
{
    Q_OBJECT

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
    Q_OBJECT

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
    Q_OBJECT

public:
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
