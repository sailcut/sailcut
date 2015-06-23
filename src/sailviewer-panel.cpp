/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
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

#include "sailviewer-panel.h"
#include "sailcpp/panelgroup.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


/**
 * Constructs a CSailViewerPanel object.
 *
 * @param parent The parent widget
 * @param viewMode The viewing mode (shaded or wireframe)
 * @param show_sliders Should the elevation and azimuth sliders be displayed?
 * @param show_labeling Should the "Labeling" button be displayed?
 */
CSailViewerPanel::CSailViewerPanel(QWidget *parent, enumViewMode viewMode, bool show_sliders, bool show_labeling)
        : QWidget(parent), showSliders(show_sliders)
{

    /* parameters groupbox */
    if (showSliders)
    {
        /* display parameters groupbox */
        grpParams = new QGroupBox( this );
        QGridLayout* grpParamsLayout = new QGridLayout( grpParams );
        lblAzimuthStatic = new QLabel( grpParams );
        grpParamsLayout->addWidget( lblAzimuthStatic, 0, 0 );
        lblAzimuth = new QLabel( grpParams );
        grpParamsLayout->addWidget( lblAzimuth, 0, 1 );
        lblElevationStatic = new QLabel( grpParams );
        grpParamsLayout->addWidget( lblElevationStatic, 1, 0 );
        lblElevation = new QLabel( grpParams );
        grpParamsLayout->addWidget( lblElevation, 1, 1 );
    }
    else
    {
        grpParams = NULL;
        lblAzimuthStatic = NULL;
        lblAzimuth = NULL;
        lblElevationStatic = NULL;
        lblElevation = NULL;
    }

    /* the drawing area */
    sailDisp = new CSailViewer(this, viewMode, showSliders);
    sailDisp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* controls groupbox */
    grpControls = new QGroupBox( this );
    QVBoxLayout *grpControlsLayout = new QVBoxLayout( grpControls );

    btnResetView = new QPushButton( grpControls );
    grpControlsLayout->addWidget( btnResetView );
    btnZoomIn = new QPushButton( grpControls );
    grpControlsLayout->addWidget( btnZoomIn );
    btnZoomOut = new QPushButton( grpControls );
    grpControlsLayout->addWidget( btnZoomOut );
    if (viewMode == WIREFRAME && show_labeling)
    {
        btnLabeling = new QPushButton( grpControls );
        grpControlsLayout->addWidget( btnLabeling );
    }
    else
    {
        btnLabeling = NULL;
    }


    /* put it all together */
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->addWidget( sailDisp );

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget( grpControls );
    vbox->addItem( new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding) );
    if (showSliders)
    {
        vbox->addWidget( grpParams );
    }
    layout->addLayout( vbox );


    /* set language and size */
    languageChange();

    /* connect signals */
    connect( sailDisp, SIGNAL( azimuthChanged(real) ), this, SLOT(slotAzimuth(real)) );
    connect( sailDisp, SIGNAL( elevationChanged(real) ), this, SLOT(slotElevation(real)) );

    if (btnLabeling)
    {
        connect( (QWidget*) btnLabeling, SIGNAL( clicked() ), sailDisp, SLOT( slotLabeling() ));
    }
    connect( (QWidget*) btnResetView, SIGNAL( clicked() ), sailDisp, SLOT( slotResetView() ));
    connect( (QWidget*) btnZoomIn, SIGNAL( clicked() ), sailDisp, SLOT( slotZoomIn() ));
    connect( (QWidget*) btnZoomOut, SIGNAL( clicked() ), sailDisp, SLOT( slotZoomOut() ));
}


/**
 * We received a keypress, we pass it down to the CSailViewer.
 */
void CSailViewerPanel::keyPressEvent ( QKeyEvent * e )
{
    sailDisp->keyPressEvent(e);
}


/**
 * Sets the strings of the subwidgets using the current language.
 */
void CSailViewerPanel::languageChange()
{
    if (showSliders)
    {
        grpParams->setTitle( tr( "Display parameters" ) );
        lblElevationStatic->setText( tr( "elevation" ) );
        lblAzimuthStatic->setText( tr( "azimuth" ) );
    }
    grpControls->setTitle( tr( "Controls" ) );
    btnResetView->setText( tr( "Reset view" ) );
    if (btnLabeling)
    {
        btnLabeling->setText( tr( "Labeling" ) );
    }
    btnZoomIn->setText( tr( "Zoom in" ) );
    btnZoomOut->setText( tr( "Zoom out" ) );
}


/**
 * Change the displayed object.
 *
 * @param obj the new object to display
 */
void CSailViewerPanel::setObject(const CPanelGroup &obj)
{
    sailDisp->setObject(obj);
}


/**
 * The azimuth was changed, update the corresponding label.
 *
 * @param azimuth
 */
void CSailViewerPanel::slotAzimuth(real azimuth)
{
    if (lblAzimuth)
    {
        lblAzimuth->setText(QString::number(azimuth) + " " +tr("deg"));
    }
}


/**
 * The elevation changed, update the corresponding label.
 *
 * @param elevation
 */
void CSailViewerPanel::slotElevation(real elevation)
{
    if (lblElevation)
    {
        lblElevation->setText(QString::number(elevation) + " " +tr("deg"));
    }
}
