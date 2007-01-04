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
    // deck parameters
    txt_DLOA->setText( QString::number(hulldef->DLOA) );
    txt_DfwdHeight->setText( QString::number(hulldef->DfwdHeight) );
    txt_DmidHeight->setText( QString::number(hulldef->DmidHeight) );
    txt_DaftHeight->setText( QString::number(hulldef->DaftHeight) );
    txt_DBW->setText( QString::number(hulldef->DBW) );
    txt_DaftW->setText( QString::number(hulldef->DaftW) );
    spinBox_DBWPos->setValue(hulldef->DBWPos);
    spinBox_StemA->setValue(hulldef->StemA);
    spinBox_DfwdShape->setValue(hulldef->DfwdShape);
    spinBox_DaftShape->setValue(hulldef->DaftShape);
    // bottom parameters
    txt_BfwdHeight->setText( QString::number(hulldef->BfwdHeight) );
    txt_BmidHeight->setText( QString::number(hulldef->BmidHeight) );
    txt_BaftHeight->setText( QString::number(hulldef->BaftHeight) );
    spinBox_BdeadriseA->setValue(hulldef->BdeadriseA);
    spinBox_BsweepA->setValue(hulldef->BsweepA);
    // planking parameters
    spinBox_NBPlank->setValue(hulldef->NBPlank);
    spinBox_TopPlankA->setValue(hulldef->TopPlankA);
    spinBox_LowPlankA->setValue(hulldef->LowPlankA);
    
    checkBox_AutoPlank->setChecked(hulldef->AutoPlank);
    
    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    //connect( checkBox_AutoPlank, SIGNAL( buttonClicked(QAbstractButton *) ), this, SLOT( slotAutoPlank() ) );
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
    
    
    /// check automatic planking 
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;
    else
        hulldef->AutoPlank = false;
    
    ///  return true IF everything is OK 
    return flag;
}

/** slot for dealing with automatic planking
 
void CFormHullDef::slotAutoPlank()
{
    if (checkBox_AutoPlank->isChecked() )
        hulldef->AutoPlank = true;
}
*/
