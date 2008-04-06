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

#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>

/** This is a generic class used as the base for various file
 *  input and output modules.
 */
template <class objtype>
class CPrinter : public QObject
{
public:
    /** The constructor.
     */
    CPrinter() {};

    /** Return the number of pages, must be overriden.
     */
    virtual size_t pages(const objtype &obj) const = 0;

    /** Perform the actual printing operation, must be overriden.
     */
    virtual void print(const objtype &obj, CTextPainter *painter, size_t page) const = 0;


    /** Display a dialog then print.
     *
     * @param obj the object to print
     * @param orientation the initial page orientation
     */
    void printDialog(const objtype &obj, enum QPrinter::Orientation orientation) const
    {
        // try printing
        try
        {
            QPrinter myprinter;
            myprinter.setOrientation(orientation);
            myprinter.setFullPage(FALSE);

            QPrintDialog printDialog(&myprinter);
            if ( printDialog.exec() == QDialog::Accepted )
            {
                CTextPainter painter(&myprinter);
                for (size_t i = 0; i < pages(obj); i ++)
                {
                    if ( i > 0 )
                        myprinter.newPage();
                    print(obj, &painter, i);
                }
            }
        }
        catch (CException e)
        {
            QMessageBox::information(NULL, tr("error"), tr("There was a printing error"));
        }
    };

};

#endif
