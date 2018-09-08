/*
 * Copyright (C) 1993-2017 Robert & Jeremy Laine
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

#include <cmath>

#include <QButtonGroup>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QLineEdit>

#include "formsaildef.h"
#include "sailcpp/sailworker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** The constructor.
 *
 * @param parent the parent window
 * @param sailptr pointer to the CSailDef
 */
CFormSailDef::CFormSailDef( QWidget* parent, CSailDef * sailptr )
        : QDialog(parent)
{
    setupUi(this);
    setModal(true);

    // we store the pointer to the CSailDef so we can update it when
    // the user clicks OK
    saildef = sailptr;

    // Write all the sail data to the screen dimensions of sail.
    setSailCut( saildef->sailCut );
    setSailType( saildef->sailType );

    txtSailID->setText(QString::fromStdString(saildef->sailID));

    txtLOA->setText( QString::number(saildef->LOA ) );
    txtTriangBase->setText( QString::number(saildef->foreJ ) );
    txtTriangHoist->setText( QString::number(saildef->foreI ) );

    txtClothWidth->setText( QString::number( saildef->clothW ) );
    txtSeamWidth->setText( QString::number( saildef->seamW ) );
    txtLeechHemWidth->setText( QString::number( saildef->leechHemW ) );
    txtFootHemWidth->setText( QString::number( saildef->footHemW ) );
    txtHemsWidth->setText( QString::number( saildef->hemsW ) );

    txtTackDist->setText( QString::number( saildef->tackX ) );
    txtTackHeight->setText( QString::number( saildef->tackY ) );
    txtRake->setText( QString::number( saildef->rake ) );

    txtLuffLen->setText( QString::number( saildef->luffL ) );
    txtLuffRound->setText( QString::number( saildef->luffR ) );
    txtLuffRoundPos->setText( QString::number( saildef->luffRP ) );

    txtGaffAngle->setText( QString::number( saildef->gaffDeg ) );
    txtGaffLen->setText( QString::number( saildef->gaffL ) );
    txtGaffRound->setText( QString::number( saildef->gaffR ) );

    txtLeechLen->setText( QString::number( saildef->leechL ) );
    txtLeechRound->setText( QString::number( saildef->leechR ) );
    txtLeechRoundPos->setText( QString::number( saildef->leechRP ) );

    txtFootLen->setText( QString::number( saildef->footL ) );
    txtFootRound->setText( QString::number( saildef->footR ) );

    txtTopDepth->setText ( QString::number( saildef->mould.profile[2].getDepth()*100 ) );
    txtMidDepth->setText ( QString::number( saildef->mould.profile[1].getDepth()*100 ) );
    txtFootDepth->setText ( QString::number( saildef->mould.profile[0].getDepth()*100 ) );

    txtTwistAngle->setText ( QString::number( saildef->twistDeg ) );
    txtSheetAngle->setText ( QString::number( saildef->sheetDeg ) );

    txtSections->setText ( QString::number( saildef->nbSections ) );
    txtGores->setText ( QString::number( saildef->nbGores ) );
    txtLuffGores->setText (QString::number( saildef->nbLuffGores ) );

    txtDihedral->setText ( QString::number( saildef->dihedralDeg ) );

    // Create button group for sail type.
    QButtonGroup *bgrpSailType = new QButtonGroup( this );
    bgrpSailType->addButton( radioMainSail );
    bgrpSailType->addButton( radioJib );
    bgrpSailType->addButton( radioWing );

    // Create button group for sail cut.
    QButtonGroup *bgrpSailCut = new QButtonGroup( this );
    bgrpSailCut->addButton( radioCross );
    bgrpSailCut->addButton( radioTwist );
    bgrpSailCut->addButton( radioHorizontal );
    bgrpSailCut->addButton( radioVertical );
    bgrpSailCut->addButton( radioMitre );
    bgrpSailCut->addButton( radioMitre2 );
    bgrpSailCut->addButton( radioRadial );

    // Set signals and slots connections
    connect( btnOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( btnCompute, SIGNAL( pressed() ), this, SLOT( slotCompute() ) );
    connect( bgrpSailType, SIGNAL( buttonClicked(QAbstractButton *) ), this, SLOT( slotSailType() ) );
    connect( bgrpSailCut, SIGNAL( buttonClicked(QAbstractButton *) ), this, SLOT( slotSailCut() ) );

    // Activate "compute" to calculate sail area and diagonal and display them.
    compute();

    // resize
    resize(minimumSizeHint());
}


// member functions

/**
 *  Compute and Display ancillary data of the sail computation
 */
void CFormSailDef::compute()
{
    CSailWorker worker(*saildef);
    lblSailArea->setText ( QString::number( worker.Area() ) );
    lblDiagonal->setText ( QString::number( worker.Diagonal() ) );
}


/**
 *  Returns the sail cut layout from the form.
 */
enumSailCut CFormSailDef::getSailCut()
{
    if ( radioHorizontal->isChecked() )
        return HORIZONTAL;
    else if ( radioMitre->isChecked() )
        return MITRE;
    else if ( radioMitre2->isChecked() )
        return MITRE2;
    else if ( radioRadial->isChecked() )
        return RADIAL;
    else if ( radioTwist->isChecked() )
        return TWIST;
    else if ( radioVertical->isChecked() )
        return VERTICAL;
    else
        /* default */
        return CROSS;
}


/**
 *  Returns the sail type from the form.
 */
enumSailType CFormSailDef::getSailType()
{
    if ( radioJib->isChecked() )
        return JIB;
    else if ( radioWing->isChecked() )
        return WING;
    else
        /* default */
        return MAINSAIL;
}


/**
 *  Enables or disables appropriate controls depending
 *  on the sail cut.
 */
void CFormSailDef::setSailCut( enumSailCut cut )
{
    switch ( cut )
    {
    case CROSS:
        radioCross->setChecked( true );
        break;
    case HORIZONTAL:
        radioHorizontal->setChecked( true );
        break;
    case RADIAL:
        radioRadial->setChecked( true );
        break;
    case TWIST:
        radioTwist->setChecked( true );
        break;
    case VERTICAL:
        radioVertical->setChecked( true );
        break;
    case MITRE:
        radioMitre->setChecked( true );
        break;
    case MITRE2:
        radioMitre2->setChecked( true );
        break;
    }
}


/**
 *  Enables or disables appropriate controls depending
 *  on the sail type.
 */
void CFormSailDef::setSailType( enumSailType type )
{
    switch ( type )
    {
    case MAINSAIL:
#ifdef DEBUG
        cout << "setSailType( MAINSAIL )" << endl;
#endif
        radioMainSail->setChecked( true );
        txtTackDist->setEnabled( true );
        txtTackHeight->setEnabled( true );
        txtRake->setEnabled( true );

        txtTriangBase->setEnabled( true );
        txtTriangHoist->setEnabled( true );

        txtGaffLen->setEnabled( true );
        txtGaffRound->setEnabled( true );
        txtGaffAngle->setEnabled( true );

        txtDihedral->setEnabled( false );
        radioCross->setEnabled( true );
        radioHorizontal->setEnabled( true );
        radioRadial->setEnabled( true );
        radioTwist->setEnabled( true );
        radioVertical->setEnabled( true );
        radioMitre->setEnabled( true );
        radioMitre2->setEnabled( true );
        break;

    case JIB:
#ifdef DEBUG
        cout << "setSailType( JIB )" << endl;
#endif
        radioJib->setChecked( true );
        txtTackDist->setEnabled( false );
        txtTackHeight->setEnabled( true );
        txtRake->setEnabled( false );

        txtTriangBase->setEnabled( true );
        txtTriangHoist->setEnabled( true );

        txtGaffLen->setEnabled( true );
        txtGaffRound->setEnabled( true );
        txtGaffAngle->setEnabled( true );

        txtDihedral->setEnabled( false );
        radioCross->setEnabled( true );
        radioHorizontal->setEnabled( true );
        radioRadial->setEnabled( true );
        radioTwist->setEnabled( true );
        radioVertical->setEnabled( true );
        radioMitre->setEnabled( true );
        radioMitre2->setEnabled( true );
        break;

    case WING:
#ifdef DEBUG
        cout << "setSailType( WING )" << endl;
#endif
        radioWing->setChecked( true );
        txtTackDist->setEnabled( true );
        txtTackHeight->setEnabled( true );
        txtRake->setEnabled( true );

        txtTriangBase->setEnabled( true );
        txtTriangHoist->setEnabled( true );

        txtGaffLen->setEnabled( false );
        txtGaffRound->setEnabled( false );
        txtGaffAngle->setEnabled( false );

        txtDihedral->setEnabled( true );
        radioHorizontal->setChecked( true );
        radioCross->setEnabled( false );
        //radioHorizontal->setEnabled( true );
        radioRadial->setEnabled( false );
        radioTwist->setEnabled( false );
        radioVertical->setEnabled( false );
        radioMitre->setEnabled( false );
        radioMitre2->setEnabled( false );
        break;

    }
}


// Qt overrides

/**
 *  Saves the parameters entered by the user in the CSailDef.
 *  Slot connected to OK button
 */
void CFormSailDef::accept()
{
    /** Return data if everything is OK. */
    if (check() == true)
        QDialog::accept();
}


/**
 *  Check all dimensions entered in order to make
 *  sure that the sail is possible and reasonable
 */
bool CFormSailDef::check()
{
    long L1=1, L2=1;
    real A1=0, A2=0;
    bool flag = true;
    QString txt;

    /** Create four palettes for levels of warning. */
    QPalette palStd, palHi, palLo, palRel;
    palStd = txtLuffLen->palette();
    palLo = palHi = palRel = palStd;   // normal value
    palLo.setColor( QPalette::Text, Qt::magenta); // too low value
    palHi.setColor( QPalette::Text, Qt::red );    // too high value
    palRel.setColor( QPalette::Text, Qt::blue );  // related value to be checked

    /** Get sail type and cut. */
    saildef->sailCut = getSailCut();
    saildef->sailType = getSailType();

    /** Get and the sail ID text length. */
    txt = txtSailID->text();
    txt = txt.simplified();

    if (txt.length() > 40)
    {
        txt.truncate(40);
        flag = false;
        txtSailID->setPalette(palHi);
        txtSailID->setText(QString(txt));
    }
    else
    {
        txtSailID->setPalette(palStd);
        txtSailID->setText(QString(txt));
    }
    saildef->sailID = txt.toStdString();

    /** Get and check length of boat. */
    saildef->LOA = txtLOA->text().toDouble();

    if (saildef->LOA < 100)
    {
        flag = false;
        txtLOA->setPalette(palLo);
        saildef->LOA = 100;
    }
    else if (saildef->LOA > 100000)
    {
        flag = false;
        txtLOA->setPalette(palHi);
        saildef->LOA = 100000;
    }
    else
    {
        txtLOA->setPalette(palStd);
    }
    txtLOA->setText(QString::number(saildef->LOA));
    L1 = (long)(saildef->LOA);

    /** Get and check foretriangle base against boat length. */
    saildef->foreJ = txtTriangBase->text().toDouble();

    if (saildef->foreJ > 0.9* L1)
    {
        flag = false;
        txtTriangBase->setPalette(palHi);
        txtLOA->setPalette(palRel);
        saildef->foreJ = floor(0.9*L1);
    }
    else   if (saildef->foreJ < 0.05* L1)
    {
        flag = false;
        txtTriangBase->setPalette(palLo);
        txtLOA->setPalette(palRel);
        saildef->foreJ = ceil(0.05*L1);
    }
    else
    {
        txtTriangBase->setPalette(palStd);
        txtLOA->setPalette(palStd);
    }
    txtTriangBase->setText(QString::number(saildef->foreJ));

    /** Get and check foretriangle hoist against boat length */
    saildef->foreI = txtTriangHoist->text().toDouble();

    if (saildef->foreI > 4*L1)
    {
        flag = false;
        txtTriangHoist->setPalette(palHi);
        txtLOA->setPalette(palRel);
        saildef->foreI = 4*L1;
    }
    else if (saildef->foreI < 0.1*L1)
    {
        flag = false;
        txtTriangHoist->setPalette(palLo);
        txtLOA->setPalette(palRel);
        saildef->foreI = ceil( 0.1*L1);
    }
    else
    {
        txtTriangBase->setPalette(palStd);
        txtLOA->setPalette(palStd);
    }
    txtTriangHoist->setText(QString::number(saildef->foreI));

    /** Check tack position function of boat length. */
    L1 = (long)(saildef->LOA);

    saildef->tackX  = txtTackDist->text().toDouble();
    if (saildef->tackX > 0.9*L1)
    {
        flag = false;
        txtTackDist->setPalette(palHi);
        txtLOA->setPalette(palRel);
        saildef->tackX = floor(0.9*L1);
    }
    else
    {
        txtTackDist->setPalette(palStd);
        txtLOA->setPalette(palStd);
    }
    txtTackDist->setText(QString::number(saildef->tackX));

    saildef->tackY  = txtTackHeight->text().toDouble();
    L1 = (long)(saildef->foreI);

    saildef->tackY  = txtTackHeight->text().toDouble();
    if (saildef->tackY > 0.5*L1)
    {
        flag = false;
        txtTackHeight->setPalette(palHi);
        txtTriangHoist->setPalette(palRel);
        saildef->tackY = floor(0.5*L1);
    }
    else
    {
        txtTackHeight->setPalette(palStd);
        txtTriangHoist->setPalette(palStd);
    }
    txtTackHeight->setText(QString::number(saildef->tackY));

    /** Get length and round data of the 4 sides of sail. */
    saildef->luffL  = txtLuffLen->text().toDouble();
    saildef->luffR  = txtLuffRound->text().toDouble();
    saildef->luffRP = txtLuffRoundPos->text().toInt();

    saildef->gaffDeg = txtGaffAngle->text().toDouble();
    saildef->gaffL  = txtGaffLen->text().toDouble();
    saildef->gaffR  = txtGaffRound->text().toDouble();

    saildef->leechL = txtLeechLen->text().toDouble();
    saildef->leechR = txtLeechRound->text().toDouble();
    saildef->leechRP = txtLeechRoundPos->text().toInt();

    saildef->footL  = txtFootLen->text().toDouble();
    saildef->footR  = txtFootRound->text().toDouble();
    saildef->footRP = 50; // imposed value

    /** Check luff rake. */
    saildef->rake   = txtRake->text().toDouble();

    switch (saildef->sailType )
    {
    case WING:
        L1 = (long)(saildef->luffL);
        if (saildef->rake > L1)
        {
            flag = false;
            txtRake->setPalette(palHi);
            saildef->rake =L1 - 1;
        }
        else if (saildef->rake < 0)
        {
            flag = false;
            txtRake->setPalette(palHi);
            saildef->rake =0;
        }
        else
        {
            txtRake->setPalette(palStd);
        }
        txtRake->setText(QString::number(saildef->rake));
        break;

    default:
        L1 = (long)(saildef->foreI);
        if (saildef->rake > 0.3*L1)
        {
            flag = false;
            txtRake->setPalette(palHi);
            txtTriangHoist->setPalette(palRel);
            saildef->rake = floor(0.3*L1 - 1);
        }
        else if (saildef->rake < -0.3*L1)
        {
            flag = false;
            txtRake->setPalette(palHi);
            txtTriangHoist->setPalette(palRel);
            saildef->rake =-floor(0.3*L1);
        }
        else
        {
            txtRake->setPalette(palStd);
            txtTriangHoist->setPalette(palStd);
        }
        txtRake->setText(QString::number(saildef->rake));
        break;
    }

    /** Check luff length and round, gaff length */
    L1 = (long)(saildef->LOA);

    switch (saildef->sailType )
    {
    case MAINSAIL:
        /** Check luff against fore triangle */
        L1 = (long) (saildef->foreI);
        if (saildef->luffL > 3*L1)
        {
            flag=false;
            txtTriangHoist->setPalette(palRel);
            txtLuffLen->setPalette(palHi);
            saildef->luffL = 3*L1;
        }
        else if (saildef->luffL < 0.1*L1)
        {
            flag=false;
            txtTriangHoist->setPalette(palRel);
            txtLuffLen->setPalette(palLo);
            saildef->luffL = 1 + 0.1*L1;
        }
        else
        {
            txtTriangHoist->setPalette(palStd);
            txtLuffLen->setPalette(palStd);
        }
        txtLuffLen->setText(QString::number(saildef->luffL));

        /** check luff round against luff length */
        L2 =(long) ((saildef->luffL ) / 10); // luff round limit

        if (saildef->luffR > L2)
        {
            flag=false;
            txtLuffRound->setPalette(palHi);
            saildef->luffR = L2;
        }
        else if (saildef->luffR <- L2  )
        {
            flag=false;
            txtLuffRound->setPalette(palLo);
            saildef->luffR = - L2 ;
        }
        else
        {
            txtLuffRound->setPalette(palStd);
        }
        txtLuffRound->setText(QString::number(saildef->luffR) );

        /** check main sail foot length against Luff*/
        L2 = (long)(saildef->luffL);
        if (saildef->footL > L2)
        {
            flag = false;
            saildef->footL = L2;
            txtFootLen->setPalette(palHi);
        }
        else if (saildef->footL < int(L2/10))
        {
            flag = false;
            saildef->footL = 1+ int(L2/10);
            txtFootLen->setPalette(palLo);
        }
        else
        {
            txtFootLen->setPalette(palStd);
        }
        txtFootLen->setText(QString::number(saildef->footL));

        /** Check main sail gaff length against foot */
        L1 =(long) (5);
        L2 =(long) (saildef->footL);

        if (saildef->gaffL < L1)
        {
            txtGaffLen->setPalette(palLo);
            saildef->gaffL= L1;
            flag = false;
        }
        else if (saildef->gaffL > L2)
        {
            txtGaffLen->setPalette(palHi);
            saildef->gaffL= L2;
            flag = false;
        }
        else
        {
            txtGaffLen->setPalette(palStd);
        }
        txtGaffLen->setText(QString::number(saildef->gaffL) );

        /** Check main sail gaff round. */
        L1 = (long) ((saildef->gaffL )/ 8);

        if (saildef->gaffR > L1)
        {
            flag=false;
            txtGaffRound->setPalette(palHi);
            saildef->gaffR = L1;
        }
        else if (saildef->gaffR <0)
        {
            flag=false;
            txtGaffRound->setPalette(palLo);
            saildef->gaffR = 0;
        }
        else
        {
            txtGaffRound->setPalette(palStd);
        }
        txtGaffRound->setText(QString::number(saildef->gaffR) );

        break;

    case JIB:
        /** check luff against fore triangle */
        L1 =( long) (sqrt(((saildef->foreI - saildef->tackY) * (saildef->foreI - saildef->tackY)) + (saildef->foreJ * saildef->foreJ ))-1);
        if (saildef->luffL > L1)
        {
            flag=false;
            txtTriangHoist->setPalette(palRel);
            txtTackHeight->setPalette(palRel);
            txtLuffLen->setPalette(palHi);
            saildef->luffL = int (L1);
        }
        else if (saildef->luffL < (L1 / 10) )
        {
            flag=false;
            txtLuffLen->setPalette(palLo);
            saildef->luffL = int(1 + real(L1) / 10);
        }
        else
        {
            txtTriangHoist->setPalette(palStd);
            txtTackHeight->setPalette(palStd);
            txtLuffLen->setPalette(palStd);
        }
        txtLuffLen->setText(QString::number(saildef->luffL) );

        L2 = (long) ((saildef->luffL )/ 10); // round limit
        /** To accommodate code zero jib
            the positive luff round limit is twice negative round limit
        */
        if (saildef->luffR > 2*L2)
        {
            flag=false;
            txtLuffRound->setPalette(palHi);
            saildef->luffR = 2*L2-1;
        }
        else if (saildef->luffR <-L2)
        {
            flag=false;
            txtLuffRound->setPalette(palLo);
            saildef->luffR = -L2+1;
        }
        else
        {
            txtLuffRound->setPalette(palStd);
        }
        txtLuffRound->setText(QString::number(saildef->luffR) );

        /** Check jib gaff length. */
        L1 =(long) (1);
        L2 =(long) (100);

        if (saildef->gaffL < L1)
        {
            txtGaffLen->setPalette(palLo);
            saildef->gaffL = L1;
            flag = false;
        }
        else if (saildef->gaffL > L2)
        {
            txtGaffLen->setPalette(palHi);
            saildef->gaffL = L2;
            flag = false;
        }
        else
        {
            txtGaffLen->setPalette(palStd);
        }
        txtGaffLen->setText(QString::number(saildef->gaffL) );

        // reset gaff round
        txtGaffRound->setPalette(palStd);
        txtGaffRound->setText(QString::number(0) );

        break;

    case WING:
        /** For a wing set gaff length to minimum. */
        saildef->gaffL = 2;
        txtGaffLen->setText(QString::number(saildef->gaffL) );
        saildef->gaffR = 0;
        txtGaffRound->setText(QString::number(saildef->gaffR) );

        // check gaff angle and set it to be horizontal
        A1 = saildef->rake / saildef->luffL;
        saildef->gaffDeg = radiansToDegrees(acos(A1));
        txtGaffAngle->setText(QString::number(floor(saildef->gaffDeg)) );

        // adjust leech length such that foot is horizontal
        A1 = saildef->rake + saildef->gaffL - saildef->footL;
        A2 = saildef->luffL * sin(degreesToRadians(saildef->gaffDeg));

        saildef->leechL = floor(sqrt( A1*A1 + A2*A2 ) );
        txtLeechLen->setText(QString::number(saildef->leechL) );

        L2 =(long) ((saildef->luffL )/ 4); // wing luff round limit

        if (saildef->luffR > L2)
        {
            flag=false;
            txtLuffRound->setPalette(palHi);
            saildef->luffR = L2;
        }
        else if (saildef->luffR <0)
        {
            flag=false;
            txtLuffRound->setPalette(palLo);
            saildef->luffR = 0;
        }
        else
        {
            txtLuffRound->setPalette(palStd);
        }
        txtLuffRound->setText(QString::number(saildef->luffR) );

        break;
    }

    /** Check leech length. */
    L1 = (long) saildef->luffL + (long) saildef->gaffL;
    L2 = (long) (int(0.5 * saildef->footL));

    if (saildef->leechL > L1+L2)
    {
        flag = false;
        txtLuffLen->setPalette(palLo);
        txtGaffLen->setPalette(palLo);
        txtFootLen->setPalette(palLo);
        txtLeechLen->setPalette(palHi);
        saildef->leechL = L1+L2;
    }
    /** leech old check
    else if (saildef->leechL > 1.5*L1)
    {
        flag = false;
        txtLuffLen->setPalette(palRel);
        txtLeechLen->setPalette(palHi);
        saildef->leechL = 1.5*L1 -1;
    }
    */

    else if (saildef->leechL < int(0.5*L1))
    {
        flag = false;
        txtLuffLen->setPalette(palRel);
        txtLeechLen->setPalette(palLo);
        saildef->leechL = 1+ int(0.5*L1);
    }
    else
    {
        txtLuffLen->setPalette(palStd);
        txtLeechLen->setPalette(palStd);
    }
    txtLeechLen->setText(QString::number(saildef->leechL));

    /** Check leech round. */
    L1 = (long) int(saildef->leechL / 10);
    L2 = (long) int(saildef->footL /5);
    if (saildef->leechR > 3*L1)
    {
        flag = false;
        txtLeechRound->setPalette(palHi);
        txtLeechRound->setText(QString::number(3*L1) );
    }
    else if (saildef->leechR <-L2)
    {
        flag = false;
        txtLeechRound->setPalette(palLo);
        txtLeechRound->setText(QString::number(1-L2) );
    }

    if (saildef->leechR <-L1)
    {
        flag = false;
        txtLeechRound->setPalette(palLo);
        txtLeechRound->setText(QString::number(-L1) );
    }
    else
        txtLeechRound->setPalette(palStd);

    /** Check foot length. */
    switch (saildef->sailType )
    {
    case MAINSAIL:
        L2 = (long)(saildef->luffL + saildef->gaffL - saildef->leechL);
        if (L2 < (long)(0.1*(saildef->luffL)))
            L2=(long)(1+ 0.1*(saildef->luffL));
        break;
    case JIB:
        L2 = (long)(saildef->luffL  - saildef->leechL);
        if (L2 < (long)(0.1*(saildef->luffL)))
            L2=(long)(1 +0.1*(saildef->luffL));
        break;
    case WING:
        L2 = (long)(saildef->luffL  - saildef->leechL);
        if (L2 < (long)(0.1*(saildef->luffL)))
            L2=(long)(1+0.1*(saildef->luffL));
        break;
    }

    if (saildef->footL > saildef->leechL)
    {
        flag=false;
        txtLeechLen->setPalette( palRel );
        txtFootLen->setPalette( palHi );
        saildef->footL = (saildef->leechL) -1;
    }
    else if (saildef->footL < L2)
    {
        flag=false;
        txtLuffLen->setPalette( palRel );
        txtLeechLen->setPalette( palHi );
        txtFootLen->setPalette( palLo );
        saildef->footL = L2 +1;
    }
    else
    {
        txtLuffLen->setPalette( palStd );
        txtLeechLen->setPalette( palStd );
        txtFootLen->setPalette( palStd );
    }
    txtFootLen->setText( QString::number( saildef->footL ) );

    /** Check foot round value. */
    L1 = (long)(saildef->footL / 5);

    switch (saildef->sailType)
    {
    case WING:
        saildef->footR = 0;
        txtFootRound->setText(QString::number(saildef->footR));
        break;

    default:

        if (saildef->footR > L1)
        {
            flag=false;
            txtFootLen->setPalette( palRel );
            txtFootRound->setPalette(palHi);
            saildef->footR = L1;
        }
        else if (saildef->footR <-L1)
        {
            flag=false;
            txtFootLen->setPalette( palRel );
            txtFootRound->setPalette(palLo);
            saildef->footR = 1-L1;
        }
        else
        {
            txtFootLen->setPalette( palStd );
            txtFootRound->setPalette(palStd );
        }
        txtFootRound->setText(QString::number(saildef->footR));
        break;
    }

    /** Get cloth, seams and hems width. */
    saildef->clothW = txtClothWidth->text().toDouble();
    saildef->seamW  = txtSeamWidth->text().toDouble();
    saildef->leechHemW = txtLeechHemWidth->text().toDouble();
    saildef->footHemW = txtFootHemWidth->text().toDouble();
    saildef->hemsW = txtHemsWidth->text().toDouble();

    /** Check cloth width against Leech length to ensure min/max number of panels. */
    if (saildef->clothW < saildef->leechL /100) // max 100 panels
    {
        saildef->clothW = 1+ int(saildef->leechL /100);
        flag = false;
        txtClothWidth->setPalette(palLo);
    }
    else if (saildef->clothW > saildef->leechL /3) // min 3 panels
    {
        saildef->clothW = int(saildef->leechL /3 -1);
        flag = false;
        txtClothWidth->setPalette(palHi);
    }
    else
    {
        txtClothWidth->setPalette(palStd);
    }
    txtClothWidth->setText( QString::number(saildef->clothW));

    L1 = (long)(5+ int(saildef->clothW / 10));

    /** Check seams width function of cloth width. */
    if (saildef->seamW > L1)
    {
        flag=false;
        txtSeamWidth->setPalette( palHi);
        saildef->seamW = L1;
    }
    else if (saildef->seamW < 0)
    {
        flag=false;
        txtSeamWidth->setPalette( palLo);
        saildef->seamW = 0;
    }
    else
    {
        txtSeamWidth->setPalette( palStd);
    }
    txtSeamWidth->setText( QString::number(saildef->seamW));

    /** Check leech hem width function of cloth width. */
    if (saildef->leechHemW > L1*2)
    {
        flag=false;
        txtLeechHemWidth->setPalette( palHi);
        saildef->leechHemW = L1*2;
    }
    else if (saildef->leechHemW  < 0)
    {
        flag=false;
        txtLeechHemWidth->setPalette( palLo);
        saildef->leechHemW = 0;
    }
    else
    {
        txtLeechHemWidth->setPalette( palStd);
    }
    txtLeechHemWidth->setText( QString::number(saildef->leechHemW));

    /** Check foot hem width function of cloth width. */
    if (saildef->footHemW > L1*2)
    {
        flag=false;
        txtFootHemWidth->setPalette( palHi);
        saildef->footHemW = L1*2;
    }
    else if (saildef->footHemW  < 0)
    {
        flag=false;
        txtFootHemWidth->setPalette( palLo);
        saildef->footHemW = 0;
    }
    else
    {
        txtFootHemWidth->setPalette( palStd);
    }
    txtFootHemWidth->setText( QString::number(saildef->footHemW));

    /** Check other luff and gaff hem width function of cloth width. */
    if (saildef->hemsW > L1)
    {
        flag=false;
        txtHemsWidth->setPalette( palHi);
        saildef->hemsW = L1;
    }
    else if (saildef->hemsW < 0)
    {
        flag=false;
        txtHemsWidth->setPalette( palLo);
        saildef->hemsW = 0;
    }
    else
    {
        txtHemsWidth->setPalette( palStd);
    }
    txtHemsWidth->setText( QString::number(saildef->hemsW));


    /** Get the mould data. */
    saildef->mould.profile[2].setDepth( real(txtTopDepth->text().toInt())/100 );
    saildef->mould.profile[1].setDepth( real(txtMidDepth->text().toInt())/100 );
    saildef->mould.profile[0].setDepth( real(txtFootDepth->text().toInt())/100 );


    /** Get and check dihedral angle for a wing. */
    saildef->dihedralDeg = txtDihedral->text().toInt();
    if (saildef->dihedralDeg<90)
    {
        flag = false;
        txtDihedral->setPalette( palLo );
        saildef->dihedralDeg = 90;
    }
    else if (saildef->dihedralDeg > 180)
    {
        flag = false;
        txtDihedral->setPalette( palHi );
        saildef->dihedralDeg = 180;
    }
    else
    {
        txtDihedral->setPalette( palStd );
    }
    txtDihedral->setText(QString::number(saildef->dihedralDeg));


    /** Get and check twist of the sail. */
    saildef->twistDeg = txtTwistAngle->text().toInt();
    if (saildef->twistDeg > 45)
    {
        flag = false;
        txtTwistAngle->setPalette( palHi );
        saildef->twistDeg = 45;
    }
    else if (saildef->twistDeg < 0)
    {
        flag = false;
        txtTwistAngle->setPalette( palLo );
        saildef->twistDeg = 0;
    }
    else
    {
        txtTwistAngle->setPalette( palStd );
    }
    txtTwistAngle->setText(QString::number(saildef->twistDeg));


    /** Get and check sheeting angle of the sail. */
    saildef->sheetDeg = txtSheetAngle->text().toInt();

    if (saildef->sheetDeg > 45)
    {
        flag=false;
        txtSheetAngle->setPalette( palHi);
        saildef->sheetDeg = 45;
    }
    else
    {
        txtSheetAngle->setPalette( palStd);
    }

    switch (saildef->sailType ) // set lower limit for sheeting main or jib
    {
    case MAINSAIL:
        L2 = 0;
        break;
    case JIB:
        L2 = 5;
        break;
    case WING:
        L2 = 0;
        break;
    }

    if (saildef->sheetDeg < L2)
    {
        flag=false;
        txtSheetAngle->setPalette( palLo);
        saildef->sheetDeg = L2;
    }
    else
    {
        txtSheetAngle->setPalette( palStd);
    }
    txtSheetAngle->setText(QString::number(saildef->sheetDeg));


    /** Get and check radial sections and gores number. */
    if (saildef->sailCut == RADIAL)
    {
        saildef->nbGores = txtGores->text().toInt();

        if (saildef->nbGores < 3)
        {
            flag=false;
            txtGores->setPalette( palLo);
            txtGores->setText(QString::number(3));
        }
        else  if (saildef->nbGores > 12)
        {
            flag=false;
            txtGores->setPalette( palHi);
            txtGores->setText(QString::number(12));
        }
        else
        {
            txtGores->setPalette( palStd);
        }
        saildef->nbGores = txtGores->text().toInt();

        saildef->nbSections = txtSections->text().toInt();

        if (saildef->nbSections < 3)
        {
            flag=false;
            txtSections->setPalette( palLo);
            txtSections->setText(QString::number(3));
        }
        else if (saildef->nbSections > 8)
        {
            flag=false;
            txtSections->setPalette( palHi);
            txtSections->setText(QString::number(8));
        }
        else
        {
            txtSections->setPalette( palStd);
        }
        saildef->nbSections = txtSections->text().toInt();

        saildef->nbLuffGores = txtLuffGores->text().toInt();

        if (saildef->nbLuffGores < 1)
        {
            flag=false;
            txtLuffGores->setPalette( palLo);
            txtLuffGores->setText(QString::number(1));
        }
        else  if (saildef->nbLuffGores > saildef->nbGores -2)
        {
            flag=false;
            txtLuffGores->setPalette( palHi);
            txtLuffGores->setText(QString::number(saildef->nbGores -2));
        }
        else
        {
            txtLuffGores->setPalette( palStd);
        }
        saildef->nbLuffGores = txtLuffGores->text().toInt();
    }

    //  return true IF everything is OK
    return flag;
}


/**
 *  Enable/disable appropriate controls when the user
 *  changes the sail cut.
 **/
void CFormSailDef::slotSailCut()
{
    setSailCut( getSailCut() );
}


/**
 *  Saves the parameters entered by the user in the CSailDef.
 *  compute and display ancillary sail data
 *
 *  slot connected to Compute button
 */
void CFormSailDef::slotCompute()
{
    real h=0, w=0, w1=0;
    QString txta, txtb, txtc, txtd, txte;

    // warning if flag returned from check is NOK
    if (check())
        txte = "\n Dimensions OK";
    else
        txte = "\n SOME DIMENSIONS ARE OUT OF LIMITS, PLEASE FIX THEM";

    // calculate sail area and diagonal
    compute();

    CSailWorker worker(*saildef);

    txta = tr("Sail corners coordinates");
    txta = txta+"\n  "+tr("tack")+" \t x = "+QString::number(int(worker.tack.x())) +"\n\t y = "+QString::number(int(worker.tack.y())) +" mm";
    txta = txta+"\n  "+tr("clew")+" \t x = "+QString::number(int(worker.clew.x())) +"\n\t y = "+QString::number(int(worker.clew.y())) +" mm";
    txta = txta+"\n  "+tr("head")+" \t x = "+QString::number(int(worker.head.x())) +"\n\t y = "+QString::number(int(worker.head.y())) +" mm";
    txta = txta+"\n  "+tr("peak")+" \t x = "+QString::number(int(worker.peak.x())) +"\n\t y = "+QString::number(int(worker.peak.y())) +" mm";

    w = worker.SailLP();
    txtb = "\nLP = " +QString::number(int(w)) +" mm ";

    txtc = "\n" + tr("Sail width measurements ");
    h=0.875;
    w = worker.IRCwidth(h);
    w1 = worker.SailWidth(h);
    // printf ("h=%f IRC width=%f IOR width=%f \n", h, w, w1);
    txtc = txtc +"\n  h= " +QString::number(h)+"\t IRC w= " +QString::number(int(w)) +" mm \t IOR w= " +QString::number(int(w1)) +" mm";

    h=0.75;
    w = worker.IRCwidth(h);
    w1 = worker.SailWidth(h);
    // printf ("h=%f IRC width=%f IOR width=%f \n", h, w, w1);
    txtc = txtc+ "\n  h= " +QString::number(h)+"\t IRC w= " +QString::number(int(w)) +" mm \t IOR w= " +QString::number(int(w1)) +" mm";

    h=0.5;
    w = worker.IRCwidth(h);
    w1 = worker.SailWidth(h);
    txtc = txtc  + "\n  h= "+QString::number(h)+"\t IRC w= "+QString::number(int(w)) +" mm \t IOR w= " +QString::number(int(w1)) +" mm";

    h=0.25;
    w = worker.IRCwidth(h);
    w1 = worker.SailWidth(h);
    txtc = txtc  + "\n  h= "+QString::number(h)+"\t IRC w= "+QString::number(int(w)) +" mm \t IOR w= " +QString::number(int(w1)) +" mm";

    displayData(  txta, txtb, txtc, txtd, txte );
}


/**
 *  Display ancillary data in a message box
 */
void CFormSailDef::displayData(QString &txt0, QString &txt1, QString &txt2, QString &txt3, QString &txt4 )
{
    QMessageBox* mb = new QMessageBox();
    mb->setIcon(QMessageBox::NoIcon);
    mb->setWindowTitle("Sailcut");
    mb->setText (QString(txt0+"\n" +txt1+"\n" +txt2+"\n" +txt3+"\n" +txt4));
    mb->setButtonText (0, "OK");
    mb->exec();
    delete mb;
}


/**
 *  Enable/disable appropriate controls when the user
 *  changes the sail type.
 **/
void CFormSailDef::slotSailType()
{
    setSailType( getSailType() );
}
