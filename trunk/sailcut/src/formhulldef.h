/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
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

#ifndef FORMHULLDEF_H
#define FORMHULLDEF_H

#include "ui_formhulldefbase.h"
#include "hulldef.h"


/** The hull definition dialog, where the user enters
 *  the parameters of the hull.
 */
class CFormHullDef : public QDialog, private Ui::CFormHullDefBase
{
    Q_OBJECT

public:
    CFormHullDef( QWidget *, CHullDef* );

    bool check();
    virtual void accept();

protected slots:
    void slotAutoPlank();
    void slotCheck();
    
protected:
    /** a pointer to the hull definion */
    CHullDef *hulldef;

};

#endif
