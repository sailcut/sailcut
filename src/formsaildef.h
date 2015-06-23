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

#ifndef FORMSAILDEF_H
#define FORMSAILDEF_H

#include "ui_formsaildefbase.h"
#include "sailcpp/saildef.h"


/** This class is for sail definition dialog where,
 *  The user enters the parameters of the sail
 *  Checking the input data
 *  Computing and displaying ancillary data.
 *
 *  @see also CsailDef for definition and default values
 */
class CFormSailDef : public QDialog, private Ui::CFormSailDefBase
{
    Q_OBJECT

public:
    CFormSailDef( QWidget *, CSailDef* );

    bool check();
    void compute();
    void displayData(QString &txt0, QString &txt1, QString &txt2, QString &txt3, QString &txt4);

    enumSailCut  getSailCut();
    enumSailType getSailType();
    void setSailCut( enumSailCut );
    void setSailType( enumSailType );

    virtual void accept();

protected slots:
    void slotSailCut();
    void slotSailType();
    void slotCompute();

protected:
    /** A pointer to the sail definion */
    CSailDef *saildef;

};

#endif
