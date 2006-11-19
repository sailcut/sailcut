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
    long I=1, J=1, L1=1, L2=1;
    //real A1=0, A2=0;
    bool flag = true;
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
    I = (long)(rigdef->foreI);
    J = (long)(rigdef->foreJ);
    if (( I / J )> 10)
    {
        flag = false;
        txt_foreI->setPalette(palHi);
        txt_foreJ->setPalette(palLo);
    }
    else if (( J / I )> 3)
    {
        flag = false;
        txt_foreJ->setPalette(palHi);
        txt_foreI->setPalette(palLo);
    }
    else
    {
        txt_foreJ->setPalette(palStd);
        txt_foreI->setPalette(palStd);
    }
    // checking mast
    rigdef->MHeight = txt_MH->text().toDouble();
    L1 = (long)(rigdef->MHeight);
    if ( L1 < I )
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_MH->setPalette(palLo);
        rigdef->MHeight = I;
    }
    else if ( L1 > (2*I))
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_MH->setPalette(palHi);
        rigdef->MHeight = ceil (2*I);
    }
    else
    {
        txt_foreI->setPalette(palStd);
        txt_MH->setPalette(palStd);
    }
    txt_MH->setText(QString::number(rigdef->MHeight));
    L1 = (long)(rigdef->MHeight);
    
    rigdef->MCord = txt_MC->text().toDouble();
    rigdef->MWidth = txt_MW->text().toDouble();
    if (rigdef->MWidth > rigdef->MCord)
    {
        flag = false;
        txt_MC->setPalette(palRel);
        txt_MW->setPalette(palHi);
        rigdef->MWidth = rigdef->MCord;
    }
    else
    {
        txt_MC->setPalette(palStd);
        txt_MW->setPalette(palStd);
    }
    txt_MW->setText(QString::number(rigdef->MWidth));
    
    rigdef->MRakeM = txt_MRM->text().toDouble();
    rigdef->MRakeD = lbl_MRD->text().toDouble();
    
    // checking shrouds
    rigdef->CSH = txt_CSH->text().toDouble();
    L2 = (long)(rigdef->CSH);
    if ( L2 > (1.5*I))
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_CSH->setPalette(palHi);
        rigdef->CSH = floor(1.5*I);
    }
    else if ( L2 < (.75*I))
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_CSH->setPalette(palLo);
        rigdef->CSH = ceil(0.75*I);
    }
    else
    {
        txt_foreI->setPalette(palStd);
        txt_CSH->setPalette(palStd);
    }
    txt_CSH->setText(QString::number(rigdef->CSH));
    L2 = (long)(rigdef->CSH);
    
    rigdef->CSB = txt_CSB->text().toDouble();
    rigdef->LSB = txt_LSB->text().toDouble();
    
    // checking spreaders
    rigdef->SPNB = spinBox_SPNB->value();
    rigdef->SPH1 = txt_SPH1->text().toDouble();
    rigdef->SPW1 = txt_SPW1->text().toDouble();
    rigdef->SPH2 = txt_SPH2->text().toDouble();
    rigdef->SPW2 = txt_SPW2->text().toDouble();
    rigdef->SPH3 = txt_SPH3->text().toDouble();
    rigdef->SPW3 = txt_SPW3->text().toDouble();
    
    // return flag = true IF everything is OK
    return flag;
}


