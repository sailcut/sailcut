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

#include <cmath>

#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QLineEdit>

#include "formhulldef.h"

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

    txt_HullID->setText(QString::fromStdString(hulldef->hullID));
    /// deck parameters
    txt_BLWL->setText( QString::number(hulldef->BLWL) );
    txt_DfwdHeight->setText( QString::number(hulldef->DfwdHeight) );
    txt_DaftHeight->setText( QString::number(hulldef->DaftHeight) );
    txt_BBW->setText( QString::number(hulldef->BBW) );
    txt_BaftW->setText( QString::number(hulldef->BaftW) );
    spinBox_BSlopeA->setValue(hulldef->BSlopeA);
    spinBox_BBWPos->setValue(hulldef->BBWPos);

    spinBox_StemA->setValue(hulldef->StemA);
    spinBox_TransomA->setValue(hulldef->TransomA);

    spinBox_BfwdShape->setValue(hulldef->BfwdShape);
    spinBox_BaftShape->setValue(hulldef->BaftShape);

    /// bottom parameters
    txt_BfwdHeight->setText( QString::number(hulldef->BfwdHeight) );
    txt_BaftHeight->setText( QString::number(hulldef->BaftHeight) );
    spinBox_BSlopeA->setValue(hulldef->BSlopeA);
    spinBox_BDeadriseA->setValue(hulldef->BDeadriseA);
    spinBox_BSweepA->setValue(hulldef->BSweepA);

    /// planking parameters
    spinBox_NBPlank->setValue(hulldef->NBPlank);
    spinBox_TopPlankA->setValue(hulldef->TopPlankA);
    spinBox_LowPlankA->setValue(hulldef->LowPlankA);

    checkBox_AutoPlank->setChecked(hulldef->AutoPlank);

    /// signal and slots connections
    connect( btnCheck, SIGNAL( clicked() ), this, SLOT( slotCheck() ) );
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

/** slot for dealing with automatic planking
 */
void CFormHullDef::slotAutoPlank()
{
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;

    check();
}

/** slot for dealing with check button
 */
void CFormHullDef::slotCheck()
{
    check();
}


/** Check all dimensions entered in order
 *  to make sure that the hull is possible and reasonable
 */
bool CFormHullDef::check()
{
    ///  return true IF everything is OK
    long L1=1;
    // real A1=0, A2=0;
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
    hulldef->hullID = txt.toStdString();

    /// check bottom data
    hulldef->BLWL = txt_BLWL->text().toDouble(); // length waterline

    if (hulldef->BLWL < 100)
    {
        flag = false;
        txt_BLWL->setPalette(palLo);
        hulldef->BLWL = 100;
    }
    else if (hulldef->BLWL > 100000)
    {
        flag = false;
        txt_BLWL->setPalette(palHi);
        hulldef->BLWL = 100000;
    }
    else
    {
        txt_BLWL->setPalette(palStd);
    }
    txt_BLWL->setText(QString::number(hulldef->BLWL));
    L1 = (long)(hulldef->BLWL);

    // lower chine height
    hulldef->BfwdHeight = txt_BfwdHeight->text().toDouble();
    if (hulldef->BfwdHeight > L1/5)
    {
        flag = false;
        txt_BfwdHeight->setPalette(palHi);
        hulldef->BfwdHeight = floor(L1 / 5);
    }
    else
    {
        txt_BfwdHeight->setPalette(palStd);
    }
    txt_BfwdHeight->setText(QString::number(hulldef->BfwdHeight));

    hulldef->BaftHeight = txt_BaftHeight->text().toDouble();
    if (hulldef->BaftHeight > L1 / 5)
    {
        flag = false;
        txt_BaftHeight->setPalette(palHi);
        hulldef->BaftHeight = floor(L1 / 5);
    }
    else
    {
        txt_BaftHeight->setPalette(palStd);
    }
    txt_BaftHeight->setText(QString::number(hulldef->BaftHeight));

    // bottom width
    hulldef->BBW = txt_BBW->text().toDouble();
    if (hulldef->BBW < L1/20)
    {
        flag = false;
        txt_BBW->setPalette(palLo);
        hulldef->BBW = ceil(L1/20);
    }
    else if (hulldef->BBW > L1/2)
    {
        flag = false;
        txt_BBW->setPalette(palHi);
        hulldef->BBW = floor(L1/2);
    }
    else
    {
        txt_BBW->setPalette(palStd);
    }
    txt_BBW->setText(QString::number(hulldef->BBW));

    hulldef->BaftW = txt_BaftW->text().toDouble();
    if (hulldef->BaftW < 0)
    {
        flag = false;
        txt_BaftW->setPalette(palLo);
        hulldef->BaftW = 0;
    }
    else if (hulldef->BaftW > hulldef->BBW)
    {
        flag = false;
        txt_BaftW->setPalette(palHi);
        hulldef->BaftW = floor(hulldef->BBW);
    }
    else
    {
        txt_BaftW->setPalette(palStd);
    }
    txt_BaftW->setText(QString::number(hulldef->BaftW));

    hulldef->BBWPos = spinBox_BBWPos->value();

    // bottom shape
    hulldef->BfwdShape = spinBox_BfwdShape->value();
    hulldef->BaftShape = spinBox_BaftShape->value();

    hulldef->BSlopeA = spinBox_BSlopeA->value();
    hulldef->BDeadriseA = spinBox_BDeadriseA->value();
    hulldef->BSweepA = spinBox_BSweepA->value();

    hulldef->StemA = spinBox_StemA->value();
    hulldef->TransomA = spinBox_TransomA->value();

    /// check deck data
    hulldef->DfwdHeight = txt_DfwdHeight->text().toDouble();
    if (hulldef->DfwdHeight < (hulldef->BfwdHeight + L1 / 20) )
    {
        flag = false;
        txt_DfwdHeight->setPalette(palLo);
        hulldef->DfwdHeight = ceil( hulldef->BfwdHeight + L1 / 20 );
    }
    else if (hulldef->DfwdHeight > ( L1 / 4 ) )
    {
        flag = false;
        txt_DfwdHeight->setPalette(palHi);
        hulldef->DfwdHeight = floor( L1 / 4 );
    }
    else
    {
        txt_DfwdHeight->setPalette(palStd);
    }
    txt_DfwdHeight->setText(QString::number(hulldef->DfwdHeight));

    hulldef->DaftHeight = txt_DaftHeight->text().toDouble();
    if (hulldef->DaftHeight < (hulldef->BaftHeight + L1 / 20) )
    {
        flag = false;
        txt_DaftHeight->setPalette(palLo);
        hulldef->DaftHeight = ceil( hulldef->BaftHeight + L1 / 20);
    }
    else if (hulldef->DaftHeight > ( L1 / 4 ) )
    {
        flag = false;
        txt_DaftHeight->setPalette(palHi);
        hulldef->DaftHeight = floor( L1 / 4 );
    }
    else
    {
        txt_DaftHeight->setPalette(palStd);
    }
    txt_DaftHeight->setText(QString::number(hulldef->DaftHeight));

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
