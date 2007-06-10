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
#include "rigworker.h"
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

    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( txt_foreI, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_foreJ, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_CSH, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_CSB, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_LSB, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_MH, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_MRnd, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_MRkM, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_MC, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_MW, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( spinBox_MRndPos, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );
    connect( txt_HAD, SIGNAL( lostFocus() ), this, SLOT( slotChanged() ) );
    connect( txt_BAD, SIGNAL( textChanged(const QString&) ), this, SLOT( slotChanged() ) );

    txt_RigID->setText(QString(rigdef->rigID) );
    txt_foreI->setText(QString::number(rigdef->foreI) );
    txt_foreJ->setText(QString::number(rigdef->foreJ) );
    txt_MH->setText(QString::number(rigdef->MHeight) );
    txt_CSH->setText(QString::number(rigdef->CSH) );
    txt_CSB->setText(QString::number(rigdef->CSB) );
    txt_LSB->setText(QString::number(rigdef->LSB) );
    txt_MRnd->setText(QString::number(rigdef->MRnd) );
    txt_MRkM->setText(QString::number(rigdef->MRakeM) );
    txt_MC->setText(QString::number(rigdef->MCord) );
    txt_MW->setText(QString::number(rigdef->MWidth) );
    spinBox_MRndPos->setValue(rigdef->MRndPos);
    // display
    lbl_MRkD->setText(QString::number(rigdef->MRakeD) );

    spinBox_SPNB->setValue(rigdef->SPNB);
    txt_SPH1->setText(QString::number(rigdef->SPH[1]) );
    txt_SPW1->setText(QString::number(rigdef->SPW[1]) );
    txt_SPH2->setText(QString::number(rigdef->SPH[2]) );
    txt_SPW2->setText(QString::number(rigdef->SPW[2]) );
    txt_SPH3->setText(QString::number(rigdef->SPH[3]) );
    txt_SPW3->setText(QString::number(rigdef->SPW[3]) );

    // variable changes triggering a check 
    txt_HAD->setText(QString::number(rigdef->HAD) );
    txt_BAD->setText(QString::number(rigdef->BAD) );

}


/** Save the parameters entered by the user in the CRigDef.
 *  slot connected to OK button
 */
void CFormRigDef::accept()
{
    // return data if everything is OK
    if (check() ==true)
        QDialog::accept();
}


/** Sets the strings of the subwidgets using the current
 *  language.
 */
void CFormRigDef::languageChange()
{   /* FIXME use proper labels 
    lbl_RigID->setText( tr("Rig identification") );
    lbl_foreJ->setText( tr("Fore triangle base") );
    lbl_foreI->setText( tr("Fore triangle height") );
    lbl_MastWidth->setText( tr("Mast width") );
    lbl_MastCord->setText( tr("Mast cord") );
    lbl_MastRakeAngle->setText( tr("Mast rake angle") );
    lbl_MastRake->setText( tr("Mast rake") );
    lbl_MastHeight->setText( tr("Mast height") );
    lbl_MastRound->setText( tr("Mast round") );
    lbl_MastRoundPos->setText( tr("Mast round position") );

    lbl_CapShroudHeight->setText( tr("Cap shroud height") );
    lbl_CapShroudBase->setText( tr("Cap shroud base width") );
    lbl_LowerShroudBase->setText( tr("Lower shroud base width") );
    lbl_SpreaderHeight1->setText( tr("Spreader height") );
    lbl_SpreaderHeight2->setText( tr("Spreader height") );
    lbl_SpreaderHeight3->setText( tr("Spreader height") );
    lbl_SpreaderWidth1->setText( tr("Spreader width") );
    lbl_SpreaderWidth2->setText( tr("Spreader width") );
    lbl_SpreaderWidth3->setText( tr("Spreader width") );
    label_SpreaderNumber->setText( tr("Number of spreaders") );

    lbl_BAD->setText( tr("Mainsail tack height") );
    lbl_HAD->setText( tr("Mainsail head height") );
    lbl_Luff_Round->setText( tr("Luff round") );
    lbl_Luff->setText( tr("Luff length") );

    */
}


/** Called when one of the values changes.
 *  it triggers a check of all data
 */
void CFormRigDef::slotChanged()
{
    /*
    if ( active == false )
        return;
    */

    check();

}


/** Check all dimensions entered in order to 
 *  make sure that the rig is possible and reasonable.
 *  This will also trigger ancillary data computation 
 *  for mainsail luff parameters.
 */
bool CFormRigDef::check()
{ 
    long I = 1, J = 1, L1 = 1, L2 = 1;
    bool flag = true;
    QString txt;
    ///  create four palettes
    QPalette palStd, palHi, palLo, palRel;
    palStd = txt_RigID->palette();
    palLo = palHi = palRel = palStd;
    palLo.setColor( QPalette::Text, Qt::magenta); // too low value
    palHi.setColor( QPalette::Text, Qt::red );    // too high value
    palRel.setColor( QPalette::Text, Qt::blue );  // related value to be checked

    //  start collecting data
    ///  check the rig ID text 
    txt = txt_RigID->text();
    txt = txt.simplified();
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
    
    /// checking fore triangle
    rigdef->foreI = txt_foreI->text().toDouble();
    if (rigdef->foreI < 100) // height too small
    {
        flag = false;
        rigdef->foreI = 100;
        txt_foreI->setPalette(palLo);
        txt_foreI->setText(QString::number(rigdef->foreI));
        return flag;
    }
    else
    {   
        txt_foreI->setPalette(palStd);
        txt_foreI->setText(QString::number(rigdef->foreI));
    }
    
    rigdef->foreJ = txt_foreJ->text().toDouble();
    I = (long)(rigdef->foreI);
    J = (long)(rigdef->foreJ);
    if (( I / J )> 10)
    {
        flag = false;
        txt_foreI->setPalette(palHi);
        txt_foreJ->setPalette(palLo);
        txt_foreI->setText(QString::number(rigdef->foreI));
        txt_foreJ->setText(QString::number(rigdef->foreJ));
        return flag;
    }
    else if (( J / I )> 2)
    {
        flag = false;
        txt_foreJ->setPalette(palHi);
        txt_foreI->setPalette(palLo);
        txt_foreI->setText(QString::number(rigdef->foreI));
        txt_foreJ->setText(QString::number(rigdef->foreJ));
        return flag;
    }
    else
    {
        txt_foreJ->setPalette(palStd);
        txt_foreI->setPalette(palStd);
        txt_foreI->setText(QString::number(rigdef->foreI));
        txt_foreJ->setText(QString::number(rigdef->foreJ));
    }
    
    /// checking mast height
    rigdef->MHeight = txt_MH->text().toDouble();
    L1 = (long)(rigdef->MHeight);
    
    if ( L1 < I )
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_MH->setPalette(palLo);
        rigdef->MHeight = I;
        return flag;
    }
    else if ( L1 > (1.8 * I))
    {
        flag = false;
        txt_foreI->setPalette(palRel);
        txt_MH->setPalette(palHi);
        rigdef->MHeight = ceil (1.8 * I);
        return flag;
    }
    else
    {
        txt_foreI->setPalette(palStd);
        txt_MH->setPalette(palStd);
    }
    txt_MH->setText(QString::number(rigdef->MHeight));
    
    L1 = (long)(rigdef->MHeight);
    
    /// checking mast cord
    rigdef->MCord = txt_MC->text().toDouble();
    if (rigdef->MCord > (L1 / 10))
    {
        flag = false;
        txt_MC->setPalette(palHi);
        rigdef->MCord = floor(L1 / 10);
    }
    else if (rigdef->MCord < (L1 / 200))
    {
        flag = false;
        txt_MC->setPalette(palLo);
        rigdef->MCord = ceil(L1 / 200);
    }
    else
    {
        txt_MC->setPalette(palStd);
    }
    txt_MC->setText(QString::number(rigdef->MCord));
    
    /// checking mast width
    rigdef->MWidth = txt_MW->text().toDouble();
    if (rigdef->MWidth > rigdef->MCord)
    {
        flag = false;
        txt_MC->setPalette(palRel);
        txt_MW->setPalette(palHi);
        rigdef->MWidth = rigdef->MCord;
    }
    else  if (rigdef->MWidth < (rigdef->MCord /5))
    {
        flag = false;
        txt_MC->setPalette(palRel);
        txt_MW->setPalette(palLo);
        rigdef->MWidth = ceil(rigdef->MCord /5);
    }
    else
    {
        txt_MC->setPalette(palStd);
        txt_MW->setPalette(palStd);
    }
    txt_MW->setText(QString::number(rigdef->MWidth));
    
    /// checking mast rake
    rigdef->MRakeM = txt_MRkM->text().toDouble();
    if ( rigdef->MRakeM > L1 / 5)
    {
        flag = false;
        rigdef->MRakeM = floor( L1 / 5 );
        txt_MH->setPalette(palRel);
        txt_MRkM->setPalette(palHi);
    } 
    else if ( rigdef->MRakeM < -L1 / 5)
    {
        flag = false;
        rigdef->MRakeM = -floor( L1 / 5 );
        txt_MH->setPalette(palRel);
        txt_MRkM->setPalette(palLo);
    } 
    else
    {
        txt_MH->setPalette(palStd);
        txt_MRkM->setPalette(palStd); 
    }
    txt_MRkM->setText(QString::number(rigdef->MRakeM));
    
    /// computing mast rake in degree
    rigdef->MRakeD = atan2(rigdef->MRakeM ,rigdef->MHeight) * (180 / PI);
    lbl_MRkD->setText(QString::number(rigdef->MRakeD));
    
    /// computing mast base distance to stem
    rigdef->MBase = rigdef->foreJ - rigdef->MRakeM * (rigdef->foreI / rigdef->MHeight);
    
    /// checking mast round
    rigdef->MRnd = txt_MRnd->text().toDouble();
    rigdef->MRndPos = spinBox_MRndPos->value();
    if ( rigdef->MRnd > L1/10 )
    {
        flag = false;
        rigdef->MRnd = floor( L1/10 );
        txt_MRnd->setPalette(palHi);
        txt_MRnd->setPalette(palHi);
    } 
    else
    {
        txt_MH->setPalette(palStd);
        txt_MRnd->setPalette(palStd); 
    }
    txt_MRnd->setText(QString::number(rigdef->MRnd));
    
    /// checking mainsail tack height 
    rigdef->BAD = txt_BAD->text().toDouble();
    if (rigdef->BAD < 0 )
    {
        flag = false;
        rigdef->BAD = 0;
        txt_BAD->setPalette(palLo);
    }
    else if ( rigdef->BAD > L1 / 2 )
    {
        flag = false;
        rigdef->BAD = int(L1 / 2);
        txt_BAD->setPalette(palHi);
        txt_MH->setPalette(palRel);
    } 
    else
    {
        txt_BAD->setPalette(palStd);
        txt_MH->setPalette(palStd); 
    }
    txt_BAD->setText(QString::number(rigdef->BAD));

    /// checking mainsail head height
    rigdef->HAD = txt_HAD->text().toDouble();
    if (rigdef->HAD < rigdef->BAD +10 )
    {
        flag = false;
        rigdef->HAD = rigdef->BAD +10;
        txt_HAD->setPalette(palLo);
        txt_BAD->setPalette(palRel);
    }
    else if ( rigdef->HAD > L1 )
    {
        flag = false;
        rigdef->HAD = L1;
        txt_HAD->setPalette(palHi);
        txt_MH->setPalette(palRel);
    } 
    else
    {
        txt_HAD->setPalette(palStd);
        txt_BAD->setPalette(palStd);
        txt_MH->setPalette(palStd); 
    }
    txt_HAD->setText(QString::number(rigdef->HAD));

    ///computing and display main sail tack and luff data
    CRigWorker worker(*rigdef);
    rigdef->MStack = worker.mastCenter( rigdef->BAD ) + CVector3d(rigdef->MCord /2, 0, 0);
    rigdef->MShead = worker.mastCenter( rigdef->HAD ) + CVector3d(rigdef->MCord /2, 0, 0);

    CVector3d MSluff = CVector3d(rigdef->MShead - rigdef->MStack);

    // compute mainsail luff round and its position 
    CPoint3d p1, p2;
    real h=0, rd1 = 0, rd2 = 0;
    unsigned int rdPos = 49;
    unsigned int i = 5;
    do 
    {
        i++;
        p1 = rigdef->MStack + MSluff*(real(i) / 50);
        h = p1.y();
        p2 = worker.mastCenter( h ) + CVector3d(rigdef->MCord /2, 0, 0);
        rd2 = CVector3d(p1-p2).x();

        if (fabs(rd2) < fabs(rd1) )
            rdPos = (i-1) * 2;
        else
            rd1 = rd2;
    }
    while ( rd1 == rd2 && i < 45);

    // display mainsail data
    lbl_MS_TackX->setText(QString::number( int(round(rigdef->MStack.x()) ))); 
    lbl_MS_TackY->setText(QString::number( int(rigdef->MStack.y()) ));
    
    lbl_MS_LuffL->setText(QString::number( int(round(MSluff.norm())) ));
    lbl_MS_Rake->setText(QString::number( int(round(MSluff.x())) ));

    lbl_MS_LuffR->setText(QString::number( round(rd1) ));
    lbl_MS_LuffRP->setText(QString::number( rdPos ) +" %");

    /// reading number of spreaders
    rigdef->SPNB = spinBox_SPNB->value();
    
    /// checking shrouds height and length
    rigdef->CSH = txt_CSH->text().toDouble();
    if ( rigdef->CSH > L1) // cap shroud above mast head
    {
        flag = false;
        txt_CSH->setPalette(palHi);
        txt_MH->setPalette(palRel);
        rigdef->CSH = L1;
        return flag;
    }
    else if ( rigdef->CSH < (0.75 * I))
    {
        flag = false;
        txt_CSH->setPalette(palLo);
        txt_foreI->setPalette(palRel);
        rigdef->CSH = ceil(0.75 * I);
        return flag;
    }
    else
    {
        txt_MH->setPalette(palStd);
        txt_foreI->setPalette(palStd);
        txt_CSH->setPalette(palStd);
    }
    txt_CSH->setText(QString::number(rigdef->CSH));
    
    rigdef->LSB = txt_LSB->text().toDouble();
    if (rigdef->LSB < (rigdef->CSH)/(10*(1 + rigdef->SPNB)) )
    {
        flag = false;
        txt_LSB->setPalette(palLo);
        txt_CSH->setPalette(palRel);
        rigdef->LSB = ceil((rigdef->CSH)/(10*(1 + rigdef->SPNB)));
    }
    else if (rigdef->LSB > ((rigdef->CSH)/(2 + rigdef->SPNB)) )
    {
        flag = false;
        txt_LSB->setPalette(palHi);
        txt_CSH->setPalette(palRel);
        rigdef->LSB = floor((rigdef->CSH)/(2 + rigdef->SPNB));
    }
    else 
    {
        txt_CSH->setPalette(palStd);
        txt_LSB->setPalette(palStd);
    }
    txt_LSB->setText(QString::number(rigdef->LSB));
    
    rigdef->CSB = txt_CSB->text().toDouble();
    if (rigdef->CSB < rigdef->LSB )
    {
        flag = false;
        txt_CSB->setPalette(palLo);
        txt_LSB->setPalette(palRel);
        rigdef->CSB = ceil(rigdef->LSB);
    }
    else if (rigdef->CSB > (2 * rigdef->LSB))
    {
        flag = false;
        txt_CSB->setPalette(palHi);
        txt_LSB->setPalette(palRel);
        rigdef->CSB = floor(2 * rigdef->LSB);
    }
    else
    {
        txt_CSB->setPalette(palStd);
        txt_LSB->setPalette(palStd);
    }
    txt_CSB->setText(QString::number(rigdef->CSB));
    
    /// checking spreaders 
    rigdef->SPH[0] = 0;
    rigdef->SPW[0] = rigdef->CSB;
    rigdef->SPH[1] = txt_SPH1->text().toDouble();
    
    if (rigdef->SPH[1] < (rigdef->CSH)/(2 + rigdef->SPNB))
    {
        flag = false;
        txt_CSH->setPalette(palRel);
        txt_SPH1->setPalette(palLo);
        rigdef->SPH[1] = ceil((rigdef->CSH)/(2 + rigdef->SPNB));
    }
    else if (rigdef->SPH[1] > ((rigdef->CSH)/(0.5 + rigdef->SPNB)) )
    {
        flag = false;
        txt_CSH->setPalette(palRel);
        txt_SPH1->setPalette(palHi);
        rigdef->SPH[1] = floor((rigdef->CSH)/(0.5 + rigdef->SPNB));
    }
    else 
    {
        txt_CSH->setPalette(palStd);
        txt_SPH1->setPalette(palStd);
    }
    txt_SPH1->setText(QString::number(rigdef->SPH[1]));
    
    rigdef->SPW[1] = txt_SPW1->text().toDouble();
    
    L2 = (long) (rigdef->CSH - rigdef->SPH[1]);
    
    rigdef->SPH[2] = txt_SPH2->text().toDouble();
    if (rigdef->SPNB>1 && rigdef->SPH[2] < (rigdef->SPH[1] +(L2 /(1 + rigdef->SPNB))) )
    {
        flag = false;
        txt_SPH1->setPalette(palRel);
        txt_SPH2->setPalette(palLo);
        rigdef->SPH[2] = ceil(rigdef->SPH[1] +(L2 /(1 + rigdef->SPNB)) );
    }
    else if (rigdef->SPNB>1 && rigdef->SPH[2] > (rigdef->SPH[1] +(L2 /(rigdef->SPNB -.5))) )
    {
        flag = false;
        txt_SPH1->setPalette(palRel);
        txt_SPH2->setPalette(palHi);
        rigdef->SPH[2] = floor(rigdef->SPH[1] + L2 /(rigdef->SPNB -.5) );
    }
    else 
    {
        txt_SPH1->setPalette(palStd);
        txt_SPH2->setPalette(palStd);
    }
    txt_SPH2->setText(QString::number(rigdef->SPH[2]));
    
    rigdef->SPW[2] = txt_SPW2->text().toDouble();
    
    L2 = (long)(rigdef->CSH - rigdef->SPH[2]);
    
    rigdef->SPH[3] = txt_SPH3->text().toDouble();
    if (rigdef->SPNB>2 && rigdef->SPH[3] < (rigdef->SPH[2] + L2 /(rigdef->SPNB)) )
    {
        flag = false;
        txt_SPH2->setPalette(palRel);
        txt_SPH3->setPalette(palLo);
        rigdef->SPH[3] = ceil(rigdef->SPH[2] + L2/(rigdef->SPNB));
    }
    else if (rigdef->SPNB>2 && rigdef->SPH[3] > (rigdef->CSH + L2/(rigdef->SPNB -1.5)) )
    {
        flag = false;
        txt_SPH2->setPalette(palRel);
        txt_SPH3->setPalette(palHi);
        rigdef->SPH[3] = floor(rigdef->CSH + L2/(rigdef->SPNB -1.5));
    }
    else 
    {
        txt_SPH2->setPalette(palStd);
        txt_SPH3->setPalette(palStd);
    }
    txt_SPH3->setText(QString::number(rigdef->SPH[3]));
    
    rigdef->SPW[3] = txt_SPW3->text().toDouble();
    
    // return flag = true IF everything is OK
    return flag;
}


