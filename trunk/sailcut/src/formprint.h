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

#ifndef FORMPRINT_H
#define FORMPRINT_H

#include <QDialog>
#include <QLabel>
#include <QPrinter>

class CPrinter;

/** A print preview label.
 */
class CPrintLabel : public QLabel
{
    Q_OBJECT

public:
    CPrintLabel(const CPrinter *printer);

protected:
    void paintEvent(QPaintEvent *event);

    size_t page;
    const CPrinter *printer;
};


/** A print preview dialog.
 */
class CFormPrint : public QDialog
{
    Q_OBJECT

public:
    CFormPrint(const CPrinter *printer, enum QPrinter::Orientation orientation);

public slots:
    void slotPrint();
    void slotSetup();

protected:
    /** display label */
    CPrintLabel *label;
    /** OK button */
    QPushButton* buttonOk;
    /** Cancel button */
    QPushButton* buttonCancel;
    /** the printer device */
    QPrinter printDevice;
    /** the print engine */
    const CPrinter *printEngine;
};

#endif
