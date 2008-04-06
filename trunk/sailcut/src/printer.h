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

#ifndef PRINTER_H
#define PRINTER_H

#include "formprint.h"
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>

class CTextPainter;


/** This is the base class used for printing.
 */
class CPrinter : public QObject
{
public:
    /** The constructor.
     */
    CPrinter() {};

    /** Return the number of pages, must be overriden.
     */
    virtual size_t pages() const = 0;

    /** Perform the actual printing operation, must be overriden.
     */
    virtual void print(CTextPainter *painter, size_t page) const = 0;

    /** Display a dialog then print.
     *
     * @param orientation the initial page orientation
     */
    void printDialog(enum QPrinter::Orientation orientation) const;
};


#endif
