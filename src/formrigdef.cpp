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

    txt_RigID->setText(QString(rigdef->rigID));
    txt_foreI->setText(QString::number(rigdef->foreI));
    txt_foreJ->setText(QString::number(rigdef->foreJ));
    txt_CSH->setText(QString::number(rigdef->CSH));
    txt_CSB->setText(QString::number(rigdef->CSB));
    txt_LSB->setText(QString::number(rigdef->LSB));
    
    txt_MH->setText(QString::number(rigdef->MHeight));
    txt_MC->setText(QString::number(rigdef->MCord));
    txt_MW->setText(QString::number(rigdef->MWidth));
    txt_MRM->setText(QString::number(rigdef->MRakeM));
    lbl_MRD->setText(QString::number(rigdef->MRakeD));
    
    spinBox_SPNB->setValue(rigdef->SPNB);
    txt_SPH1->setText(QString::number(rigdef->SPH1));
    txt_SPW1->setText(QString::number(rigdef->SPW1));
    txt_SPH2->setText(QString::number(rigdef->SPH2));
    txt_SPW2->setText(QString::number(rigdef->SPW2));
    txt_SPH3->setText(QString::number(rigdef->SPH3));
    txt_SPW3->setText(QString::number(rigdef->SPW3));

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
    palStd = txt_RigID->palette();
    palLo = palHi = palRel = palStd;
    palLo.setColor( QPalette::Text, Qt::magenta); // too low value
    palHi.setColor( QPalette::Text, Qt::red );    // too high value
    palRel.setColor( QPalette::Text, Qt::blue );  // related value to be checked

    ///  start collecting data
    txt = txt_RigID->text();
    txt = txt.simplified();

    ///  check the rig ID text
    if (txt.length() > 40)
    {
        txt.truncate(40);
        flag = false;
        txt_RigID->setPalette(palHi);
        txt_RigID->setText(QString(txt));
    }
    else
    {
        txt_RigID->setPalette(palStd);
        txt_RigID->setText(QString(txt));
    }
    rigdef->rigID = txt;
    
    rigdef->foreI = txt_foreI->text().toDouble();
    rigdef->foreJ = txt_foreJ->text().toDouble();

    
    // return true IF everything is OK
    return true;
}


