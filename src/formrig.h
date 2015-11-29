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

#ifndef FORMRIG_H
#define FORMRIG_H

#include "formdocument.h"
#include "sailwriter-xml.h"

/** Dialog holding a rig.
 */
class CFormRig : public CFormDocumentTmpl<CRigDef, CRigDefXmlWriter>
{
    Q_OBJECT

public:
    CFormRig(QWidget *parent);

protected:
    T_KEYPRESS
    void setDef(const CRigDef& newdef);
    void setupMenuBar();

    // slots
protected slots:
    virtual void slotDef();

private slots:
    void languageChange();

    // member variables
protected:
    /** view hull definition */
    QAction *actionViewDef;
};

#endif
