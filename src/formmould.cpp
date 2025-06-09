/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
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

#include "formmould.h"
#include "widgetprofile.h"
#include "widgetprofilevert.h"
#include "sailcpp/sailmould.h"

#include <QLabel>
#include <QFrame>
#include <QLayout>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>


/** The constructor.
 *
 * @param parent the parent dialog
 * @param mouldptr pointer to the CSailMould
 */
CFormMould::CFormMould( QWidget *parent, CSailMould *mouldptr )
        : QDialog(parent)
{
    setModal(true);
    setWindowTitle( tr( "Sail mould" ) );

    // we store the pointer to the CSailMould so we can update it when
    // the user clicks OK
    sailmould = mouldptr;

    setSizeGripEnabled( true );
    QGridLayout* CFormMouldLayout = new QGridLayout( this );

    // Add the buttons at the bottom of the screen
    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    buttonsLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

    buttonOk = new QPushButton( tr("&OK"), this );
    buttonOk->setDefault( true );
    buttonsLayout->addWidget( buttonOk );

    buttonCancel = new QPushButton( tr("&Cancel"), this );
    buttonsLayout->addWidget( buttonCancel );

    CFormMouldLayout->addLayout( buttonsLayout, 1, 1 );

    // add vertical repartition
    widgetVert = new CWidgetProfileVert( this, sailmould, tr("Vertical repartition") );

    CFormMouldLayout->addWidget( widgetVert, 0, 0 );

    // add 3 profile areas and bind them to their respective profiles
    QFrame* frmProfile = new QFrame( this );
    frmProfile->setFrameShape( QFrame::NoFrame );
    frmProfile->setFrameShadow( QFrame::Raised );
    QGridLayout* frmProfileLayout = new QGridLayout( frmProfile );
    CFormMouldLayout->addWidget( frmProfile, 0, 1 );

    prfTop = new CWidgetProfile( frmProfile, &sailmould->profile[2], tr("Top profile"), widgetVert );
    prfMiddle = new CWidgetProfile( frmProfile, &sailmould->profile[1], tr("Middle profile"), widgetVert );
    prfBottom = new CWidgetProfile( frmProfile, &sailmould->profile[0], tr("Bottom profile"), widgetVert );

    frmProfileLayout->addWidget( prfTop, 0, 0 );
    frmProfileLayout->addWidget( prfMiddle, 1, 0 );
    frmProfileLayout->addWidget( prfBottom, 2, 0 );

    // init
    //languageChange();
    resize( QSize(487, 555).expandedTo(minimumSizeHint()) );

    // setting the range for profile spin boxes is done in widgetprofile.cpp
    prfBottom->spinDepth->setMinimum( 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

