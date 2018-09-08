/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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

#ifndef FORMRIGDEF_H
#define FORMRIGDEF_H

#include "ui_formrigdefbase.h"
#include "sailcpp/rigdef.h"


/** The rig definition dialog, where the user enters
 *  the parameters of the rig.
 *
 *  @see CRigDef default values
 */
class CFormRigDef : public QDialog, private Ui::CFormRigDefBase
{
    Q_OBJECT

public:
    CFormRigDef( QWidget *, CRigDef* );

    bool check();
    virtual void accept();

protected slots:
    void slotChanged();

private slots:
    void languageChange();

protected:
    /** a pointer to the rig definion */
    CRigDef *rigdef;

};

#endif
