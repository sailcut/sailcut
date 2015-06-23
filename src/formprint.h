/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
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

#ifndef FORMPRINT_H
#define FORMPRINT_H

#include <QDialog>
#include <QLabel>
#include <QPrinter>

class CFormPrint;
class CPrinter;
class QDoubleSpinBox;
class QToolButton;

/** A print preview label.
 */
class CPrintLabel : public QLabel
{
    Q_OBJECT

public:
    CPrintLabel(CFormPrint *form);
    /** Get the current page. */
    int getPage()
    {
        return page;
    };
    double getScale()
    {
        return scale;
    };
    void setPage(int page);

public slots:
    void slotPagePrev();
    void slotPageNext();
    void slotScale(double scale);

protected:
    void resizeEvent(QResizeEvent * event);
    void paintEvent(QPaintEvent *event);

    /** scale */
    double scale;
    /** the current page number */
    int page;
    /** the print preview form */
    CFormPrint *form;
    /** is the label currently being resized? */
    bool resizing;
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

protected:
    /** display label */
    CPrintLabel *label;

    /** scale label */
    QLabel *labelScale;
    /** scale edit */
    QDoubleSpinBox *spinScale;

    /** left button */
    QToolButton *buttonLeft;
    /** page label */
    QLabel *labelPage;
    /** right button */
    QToolButton *buttonRight;

    /** OK button */
    QPushButton *buttonOk;
    /** Cancel button */
    QPushButton *buttonCancel;

    /** the printer device */
    QPrinter printDevice;
    /** the print engine */
    const CPrinter *printEngine;
    /** the print font size */
    int printFontSize;

    friend class CPrintLabel;
};

#endif
