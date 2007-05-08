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

#include "widgetprofile.h"
#include "widgetprofilevert.h"
#include "sailmould.h"
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>
#include <QSpinBox>
#include <QPainter>


/**************************************************************************

                         CLabelProfile class

**************************************************************************/

/** The constructor.
 *
 * @param parent the parent dialog
 * @param ptr pointer to the CProfile
 */
CLabelProfile::CLabelProfile( QWidget *parent, CProfile *ptr)
        : QLabel(parent), CDispArea(0.95), profile(ptr)
{
    // set the background to white
    QPalette pal = palette();
    pal.setColor( QPalette::Background, Qt::white );
    setPalette( pal );
}


/** Display the CProfile in the drawing area.
 */
void CLabelProfile::paintEvent( QPaintEvent *)
{
    QPainter painter( this );
    QRect vRect = painter.viewport();
    painter.eraseRect(vRect);

    if ( wasResized )
    {
        CRect3d viewRect;
        viewRect.max = CPoint3d(vRect.width(), vRect.height());

        CRect3d objRect;
        objRect.max = CPoint3d(1, 0.2);
        center = objRect.center();

        lRect = calcLRect(viewRect, objRect);
        wasResized = 0;
    }

    QPoint prev, next;
    unsigned int nbpoints = 32;
    real x, z;

    real scale =  vRect.height() / lRect.height();

    // do a translation to have from z=0 to z=scale centered
    painter.translate( (lRect.width() / 2 - center.x()) * scale,  vRect.height() + (center.y() - lRect.height()/2) * scale );

    // flip coordinate system to have the z axis pointing up
    painter.scale(1,-1);

    prev = QPoint( 0, int(scale * profile->z(0)) );
    for( unsigned i = 1; i < nbpoints; i++)
    {
        x = real(i) / (nbpoints-1);
        z = profile->z(x);

        //qDebug( "( " + QString::number(x) + ", " + QString::number(z) + " )" );
        next = QPoint( int(x*scale) , int(z*scale) );
        painter.drawLine(prev, next);
        prev = next;
    }
}


/** The draw area has been resized.
 */
void CLabelProfile::resizeEvent( QResizeEvent * )
{
    wasResized = 1;
}



/**************************************************************************

                         CWidgetProfile class

**************************************************************************/

/** The constructor.
 *
 * @param parent the parent dialog
 * @param ptr a pointer to the profile we are displaying / editing
 * @param caption caption for the profile view (e.g. top,middle..)    QLabel* lblLeech;

 * @param vert the vertical repartition widget
 */
CWidgetProfile::CWidgetProfile( QWidget *parent, CProfile *ptr,
                                QString caption , CWidgetProfileVert *vert)
        : QWidget(parent), widgetVert(vert)
{
    profile = ptr;

    QHBoxLayout *layout = new QHBoxLayout( this );

    QGroupBox *grpProfile = new QGroupBox( this );
    //    grpProfile->setAlignment( int( QGroupBox::AlignCenter | QGroupBox::AlignRight | QGroupBox::AlignLeft ) );
    //    grpProfile->setColumnLayout(0, Qt::Vertical );

    QGridLayout *grpProfileLayout = new QGridLayout( grpProfile );
    grpProfileLayout->setAlignment( Qt::AlignTop );
    grpProfile->setTitle(caption);

    //// add the spin boxes for changing profile
    QHBoxLayout *spinBoxesLayout = new QHBoxLayout();

    // luff factor
    lblLuff = new QLabel( grpProfile );
    lblLuff->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    spinBoxesLayout->addWidget( lblLuff );

    spinLuff = new QSpinBox( grpProfile );
    spinLuff->setMaximum( 16 );
    spinLuff->setMinimum( 1 );
    spinLuff->setValue( 1 );
    spinBoxesLayout->addWidget( spinLuff );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
    spinBoxesLayout->addItem( spacer );

    // depth
    lblDepth = new QLabel( grpProfile );
    lblDepth->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    spinBoxesLayout->addWidget( lblDepth );

    spinDepth = new QSpinBox( grpProfile );
    spinDepth->setMaximum( 20 );
    spinDepth->setMinimum( 1 );
    spinDepth->setValue( 10 );
    spinBoxesLayout->addWidget( spinDepth );

    lblPercent = new QLabel( grpProfile );
    spinBoxesLayout->addWidget( lblPercent );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
    spinBoxesLayout->addItem( spacer_2 );

    // leech factor
    lblLeech = new QLabel( grpProfile );
    spinBoxesLayout->addWidget( lblLeech );

    spinLeech = new QSpinBox( grpProfile );
    spinLeech->setMinimum( -4 );
    spinLeech->setMaximum( 14 );
    spinBoxesLayout->addWidget( spinLeech );

    grpProfileLayout->addLayout( spinBoxesLayout, 1, 0 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    grpProfileLayout->addItem( spacer_3, 0, 0 );

    //// add the additional data labels
    QHBoxLayout *dataBoxesLayout = new QHBoxLayout();

    lblLuffSlopeValue = new QLabel(grpProfile);
    dataBoxesLayout->addWidget (lblLuffSlopeValue);
    lblLuffSlopeDegrees = new QLabel(grpProfile);
    dataBoxesLayout->addWidget (lblLuffSlopeDegrees);
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
    dataBoxesLayout->addItem( spacer_5 );

    lblMaxPosValue = new QLabel(grpProfile);
    dataBoxesLayout->addWidget (lblMaxPosValue);
    lblMaxPosCord = new QLabel(grpProfile);
    dataBoxesLayout->addWidget (lblMaxPosCord);
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
    dataBoxesLayout->addItem( spacer_6 );

    lblLeechSlopeValue = new QLabel(grpProfile);
    dataBoxesLayout->addWidget (lblLeechSlopeValue);
    lblLeechSlopeDegrees = new QLabel(grpProfile);
    dataBoxesLayout->addWidget (lblLeechSlopeDegrees);

    grpProfileLayout->addLayout( dataBoxesLayout, 2, 0 );

    /// create the drawing area
    lblDraw = new CLabelProfile( grpProfile, profile );
    grpProfileLayout->addWidget( lblDraw, 0, 0 );

    // add the group box that contains all the above elements
    layout->addWidget( grpProfile );

    // we set the "active" flag to false so that we can set the initial
    // values without slotChanged being triggered
    active = false;
    spinLeech->setValue( int(profile->getLeech()*50 +0.01) );//+0.01 to avoid rounding down
    spinLuff->setValue( int(profile->getLuff()) );
    spinDepth->setValue( int(profile->getDepth()*100 +0.01) );//+0.01 to avoid rounding down
    active = true;

    // signals and slots connections
    connect( spinLuff, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );
    connect( spinDepth, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );
    connect( spinLeech, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );

    // set translations and trigger update 
    // NOTE : all the widgets need to be created before doing this
    languageChange();
    resize( QSize(527, 159).expandedTo(minimumSizeHint()) );
    
    // trigger the computation of profile to update the ancillary data labels
    slotChanged();
}


/** Sets the strings of the subwidgets using the current
 *  language.
 */
void CWidgetProfile::languageChange()
{
    lblLuff->setText( tr( "Luff shape" ) );
    lblDepth->setText( tr( "Depth" ) );
    lblPercent->setText( tr( "%" ) );
    lblLeech->setText( tr( "Leech shape" ) );
    // ancillary data labels
    lblLuffSlopeDegrees->setText( tr( "degrees" ) );
    lblMaxPosCord->setText( tr( "cord") );
    lblLeechSlopeDegrees->setText( tr( "degrees" ) );    
}


/** Called when one of the spinboxes changes value.
 */
void CWidgetProfile::slotChanged()
{
    if (active == false)
        return;

    // store the new values
    *profile = CProfile( real(spinDepth->value())/100, real(spinLeech->value())/50, spinLuff->value() );

    // update profile view
    lblDraw->update();
    // display the position of the maximum depth 
    lblLuffSlopeValue->setText( QString::number ( int( 57.29579 * atan(profile->slope(0.0)) ) ));
    lblMaxPosValue->setText( QString::number (profile->getMaxPos() ) );
    lblLeechSlopeValue->setText( QString::number ( int( 57.29579 * atan(profile->slope(1.0)) ) ) );

    // update the vertical repartition view
    widgetVert->updateLabel();
}

