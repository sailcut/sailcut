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

#include "widgetprofilevert.h"
#include "sailcalc.h"
#include "sailmould.h"

#include <QGroupBox>
#include <QLayout>
#include <QPainter>
#include <QSlider>

/**************************************************************************

                         CLabelProfileVert class

**************************************************************************/

/** The constructor.
 *
 * @param parent the parent dialog
 * @param ptr pointer to the CSailMould
 */
CLabelProfileVert::CLabelProfileVert( QWidget *parent, CSailMould *ptr)
        : QLabel(parent), mould(ptr), wasResized(true)
{
    // set the background to white
    QPalette pal = palette();
    pal.setColor( QPalette::Background, Qt::white );
    setPalette( pal );

    setMinimumSize( QSize( 110, 400 ) );
}


/** Display the vertical repartition in the drawing area.
 */
void CLabelProfileVert::paintEvent( QPaintEvent *)
{
    QPainter painter( this );

    QRect vRect = painter.viewport();
    painter.eraseRect(vRect);

    CRect3d objRect;
    objRect.max = CPoint3d(0.2, 1);

    if ( wasResized )
    {
        CRect3d viewRect;
        viewRect.max = CPoint3d(vRect.width(), vRect.height());

        m_lRect = calcLRect(viewRect, objRect, objRect.center(), 0.8);
        wasResized = 0;
    }

    QPoint prev, next;
    unsigned int nbpoints = 32;
    real z, y; // z is horizontal and y is vertical upward

    real scale =  vRect.height() / m_lRect.height();

    // do a translation to have from z=0 to z=scale centered
    painter.translate( (m_lRect.width() / 2 - objRect.center().x()) * scale,  vRect.height() + (objRect.center().y() - m_lRect.height()/2) * scale );

    // flip coordinate system to have the y axis pointing up
    painter.scale(1,-1);

    prev = QPoint( int(scale * mould->interpol(0).getDepth()), 0 );
    for (unsigned i = 1; i < nbpoints; i++)
    {
        y = real(i) / (nbpoints-1);
        z = mould->interpol(y).getDepth();

        next = QPoint( int(scale * z) , int(scale * y));
        painter.drawLine(prev, next);
        prev = next;
    }

}


/** The draw area has been resized.
 */
void CLabelProfileVert::resizeEvent( QResizeEvent * )
{
    wasResized = 1;
}


/**************************************************************************

                         CWidgetProfileVert class

**************************************************************************/

/** The constructor.
 *
 * @param parent the parent dialog
 * @param mouldptr pointer to the CSailMould
 * @param caption caption for the profile view (e.g. top,middle..)
 */
CWidgetProfileVert::CWidgetProfileVert(QWidget *parent, CSailMould *mouldptr, QString caption)
        : QWidget(parent), sailmould(mouldptr)
{
    QHBoxLayout *layout = new QHBoxLayout( this );

    grpVertical = new QGroupBox( this );
    grpVertical->setTitle(caption);
    QVBoxLayout *grpVerticalLayout = new QVBoxLayout(grpVertical);

    ////////////
    QHBoxLayout *drawVerticalHBox = new QHBoxLayout();

    // add depth vertical repartition draw area
    lblDrawVert = new CLabelProfileVert( grpVertical, sailmould );
    drawVerticalHBox->addWidget( lblDrawVert );

    // add vertical slider for mid depth position
    sliderVertical = new QSlider( grpVertical );
    sliderVertical->setMinimum( 30 );
    sliderVertical->setMaximum( 70 );
    sliderVertical->setOrientation( Qt::Vertical );
    sliderVertical->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //sliderVertical->setValue( 100 - sailmould->vertpos );
    sliderVertical->setValue( sailmould->vertpos );

    drawVerticalHBox->addWidget( sliderVertical );

    grpVerticalLayout->addItem( drawVerticalHBox );
    /////////////
    QHBoxLayout *lblVerticalHBox = new QHBoxLayout();

    lblVertical = new QLabel( grpVertical );
    lblVertical->setAlignment( Qt::AlignCenter );
    lblVerticalHBox->addWidget( lblVertical );

    grpVerticalLayout->addItem( lblVerticalHBox );

    /////////////

    layout->addWidget( grpVertical );

    slotSliderVert();

    connect( sliderVertical, SIGNAL( valueChanged(int) ), this, SLOT( slotSliderVert() ) );

}


/** Trigered when the user moves the vertical repartition slider. */
void CWidgetProfileVert::slotSliderVert()
{
    // store new value of vertical position of mid profile and update label
    //sailmould->vertpos = 100 - sliderVertical->value();
    sailmould->vertpos = sliderVertical->value();
    lblVertical->setText( QString::number(sailmould->vertpos) );

    // update vertical profile view
    lblDrawVert->update();
}
