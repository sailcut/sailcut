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

#include "formhulldef.h"
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QLineEdit>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** The constructor.
 *
 * @param parent the parent window
 * @param hullptr pointer to the CHullDef
 */
CFormHullDef::CFormHullDef( QWidget* parent, CHullDef * hullptr )
        : QDialog(parent)
{
    setupUi(this);
    setModal(true);

    /* we store the pointer to the CHullDef so we can update it when
       the user clicks OK */
    hulldef = hullptr;

    txt_HullID->setText(QString(hulldef->hullID));
    /// deck parameters
    txt_DLOA->setText( QString::number(hulldef->DLOA) );
    txt_DfwdHeight->setText( QString::number(hulldef->DfwdHeight) );
    txt_DaftHeight->setText( QString::number(hulldef->DaftHeight) );
    txt_DBW->setText( QString::number(hulldef->DBW) );
    txt_DaftW->setText( QString::number(hulldef->DaftW) );
    spinBox_DSlopeA->setValue(hulldef->DSlopeA);
    spinBox_DBWPos->setValue(hulldef->DBWPos);
    
    spinBox_StemA->setValue(hulldef->StemA);
    spinBox_TransomA->setValue(hulldef->TransomA);
    
    spinBox_DfwdShape->setValue(hulldef->DfwdShape);
    spinBox_DaftShape->setValue(hulldef->DaftShape);
    
    /// bottom parameters
    txt_BfwdHeight->setText( QString::number(hulldef->BfwdHeight) );
    txt_BaftHeight->setText( QString::number(hulldef->BaftHeight) );
    spinBox_BSlopeA->setValue(hulldef->BSlopeA);
    spinBox_BdeadriseA->setValue(hulldef->BdeadriseA);
    spinBox_BsweepA->setValue(hulldef->BsweepA);
    
    /// planking parameters
    spinBox_NBPlank->setValue(hulldef->NBPlank);
    spinBox_TopPlankA->setValue(hulldef->TopPlankA);
    spinBox_LowPlankA->setValue(hulldef->LowPlankA);
    
    checkBox_AutoPlank->setChecked(hulldef->AutoPlank);
    
    /// signal and slots connections
    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}


// Qt overrides

/** Saves the parameters entered by the user in the CHullDef.
 *  slot connected to OK button
 */
void CFormHullDef::accept()
{
    // return data if everything is OK. /////////////////////////
    if (check() == true)
        QDialog::accept();
}


/** Check all dimensions entered in order
 *  to make sure that the hull is possible and reasonable
 */
bool CFormHullDef::check()
{
    ///  return true IF everything is OK
    long L1=1, L2=1;
    real A1=0, A2=0;
    bool flag = true;
    QString txt;
    
    ///  create four palettes
    QPalette palStd, palHi, palLo, palRel;
    palStd = txt_HullID->palette();
    palLo = palHi = palRel = palStd;
    palLo.setColor( QPalette::Text, Qt::magenta); // too low value
    palHi.setColor( QPalette::Text, Qt::red );    // too high value
    palRel.setColor( QPalette::Text, Qt::blue );  // related value to be checked
    
    /// check hull ID
    txt = txt_HullID->text();
    txt = txt.simplified();
    if (txt.length() > 40)
    {
        txt.truncate(40);
        flag = false;
        txt_HullID->setPalette(palHi);
        txt_HullID->setText(QString(txt));
    }
    else
    {
        txt_HullID->setPalette(palStd);
        txt_HullID->setText(QString(txt));
    }
    hulldef->hullID = txt;
    
    /// check deck data
    hulldef->DLOA = txt_DLOA->text().toDouble();

    if (hulldef->DLOA < 100)
    {
        flag = false;
        txt_DLOA->setPalette(palLo);
        hulldef->DLOA = 100;
    }
    else if (hulldef->DLOA > 100000)
    {
        flag = false;
        txt_DLOA->setPalette(palHi);
        hulldef->DLOA = 100000;
    }
    else
    {
        txt_DLOA->setPalette(palStd);
    }
    txt_DLOA->setText(QString::number(hulldef->DLOA));
    L1 = (long)(hulldef->DLOA);
    
    hulldef->DfwdHeight = txt_DfwdHeight->text().toDouble();
    hulldef->DaftHeight = txt_DaftHeight->text().toDouble();
    hulldef->DSlopeA = spinBox_DSlopeA->value();
    
    hulldef->DBW = txt_DBW->text().toDouble();
    if (hulldef->DBW < L1/20)
    {
        flag = false;
        txt_DBW->setPalette(palLo);
        hulldef->DBW = ceil(L1/20);
    }
    else if (hulldef->DBW > L1/2)
    {
        flag = false;
        txt_DBW->setPalette(palHi);
        hulldef->DBW = floor(L1/2);
    }
    else
    {
        txt_DBW->setPalette(palStd);
    }
    txt_DBW->setText(QString::number(hulldef->DBW));
    
    hulldef->DaftW = txt_DaftW->text().toDouble();
    if (hulldef->DaftW < 0)
    {
        flag = false;
        txt_DaftW->setPalette(palLo);
        hulldef->DaftW = 0;
    }
    else if (hulldef->DaftW > hulldef->DBW)
    {
        flag = false;
        txt_DaftW->setPalette(palHi);
        hulldef->DaftW = floor(hulldef->DBW);
    }
    else
    {
        txt_DaftW->setPalette(palStd);
    }
    txt_DaftW->setText(QString::number(hulldef->DaftW));
    
    hulldef->DBWPos = spinBox_DBWPos->value();
    
    hulldef->StemA = spinBox_StemA->value();
    hulldef->TransomA = spinBox_TransomA->value();
    
    hulldef->DfwdShape = spinBox_DfwdShape->value();
    hulldef->DaftShape = spinBox_DaftShape->value();
    
    /// check bottom data
    hulldef->BfwdHeight = txt_BfwdHeight->text().toDouble();
    if (hulldef->BfwdHeight > hulldef->DfwdHeight - L1/10)
    {
        flag = false;
        txt_BfwdHeight->setPalette(palHi);
        hulldef->BfwdHeight = floor(hulldef->DfwdHeight - L1/10);
    }
    else
    {
        txt_BfwdHeight->setPalette(palStd);
    }
    txt_BfwdHeight->setText(QString::number(hulldef->BfwdHeight));
    
    hulldef->BaftHeight = txt_BaftHeight->text().toDouble();
    if (hulldef->BaftHeight > hulldef->DaftHeight - L1/10)
    {
        flag = false;
        txt_BaftHeight->setPalette(palHi);
        hulldef->BaftHeight = floor(hulldef->DaftHeight - L1/10);
    }
    else
    {
        txt_BaftHeight->setPalette(palStd);
    }
    txt_BaftHeight->setText(QString::number(hulldef->BaftHeight));
    
    hulldef->BSlopeA = spinBox_BSlopeA->value();
    hulldef->BdeadriseA = spinBox_BdeadriseA->value();
    hulldef->BsweepA = spinBox_BsweepA->value();
    
    /// planking parameters
    hulldef->NBPlank = spinBox_NBPlank->value();
    hulldef->TopPlankA = spinBox_TopPlankA->value();
    hulldef->LowPlankA = spinBox_LowPlankA->value();
    
    /// check automatic planking 
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;
    else
        hulldef->AutoPlank = false;
    
    ///  return true IF everything is OK 
    return flag;
}

/** slot for dealing with automatic planking
 */ 
void CFormHullDef::slotAutoPlank()
{
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;
        
    check();
}

