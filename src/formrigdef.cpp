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

#include "formrigdef.h"
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>


/** The constructor.
 *
 * @param parent the parent window
 * @param rigptr pointer to the CRigDef
 */
CFormRigDef::CFormRigDef( QWidget* parent, CRigDef * rigptr )
        : QDialog(parent)
{
    setupUi(this);
    setModal(true);

    /* we store the pointer to the CRigDef so we can update it when
       the user clicks OK */
    rigdef = rigptr;

    //txtRigID->setText(QString(rigdef->rigID));

    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}


/** Saves the parameters entered by the user in the CRigDef.
 *  slot connected to OK button
 */
void CFormRigDef::accept()
{
    // return data if everything is OK
    if (check() ==true)
        QDialog::accept();
}


/** Check all dimensions entered in order
 *  to make sure that the rig is possible and reasonable.
 */
bool CFormRigDef::check()
{ 
    long L1=1, L2=1;
    real A1=0, A2=0;
    bool flag=true;
    QString txt;
    ///  create four palettes
    QPalette palStd, palHi, palLo, palRel;
    palStd = txtRigID->palette();
    palLo = palHi = palRel = palStd;
    palLo.setColor( QPalette::Text, Qt::magenta); // too low value
    palHi.setColor( QPalette::Text, Qt::red );    // too high value
    palRel.setColor( QPalette::Text, Qt::blue );  // related value to be checked

    ///  start collecting data
    txt = txtRigID->text();
    txt = txt.simplified();

    ///  check the rig ID text
    if (txt.length() > 40)
    {
        txt.truncate(40);
        flag = false;
        txtRigID->setPalette(palHi);
        txtRigID->setText(QString(txt));
    }
    else
    {
        txtRigID->setPalette(palStd);
        txtRigID->setText(QString(txt));
    }
    rigdef->rigID = txt;
    
    rigdef->foreI = txtI->text().toDouble();
    rigdef->foreJ = txtJ->text().toDouble();

    
    // return true IF everything is OK
    return true;
}


