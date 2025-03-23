/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
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

#include "sailviewer.h"
#include "saildisplabel.h"
#include "saildispgl.h"
#include <QSlider>
#include <QLayout>
#include <QKeyEvent>

/** Constructs a CSailViewer object.
 */
CSailViewer::CSailViewer( QWidget *parent, enumViewMode viewMode, bool show_sliders ) :
        QWidget(parent)
{
    sailDispLayout = new QGridLayout( this );

    int xpos = 0, ypos = 0;

    if (show_sliders)
    {
        sliderElevation = new QSlider( this );
        sliderElevation->setMinimum( -180 );
        sliderElevation->setMaximum( 180 );
        sliderElevation->setOrientation( Qt::Vertical );
        sliderElevation->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sailDispLayout->addWidget( sliderElevation, 0, 0 );
        xpos++;

        sliderAzimuth = new QSlider( this );
        sliderAzimuth->setMinimum( -180 );
        sliderAzimuth->setMaximum( 180 );
        sliderAzimuth->setOrientation( Qt::Horizontal );
        sliderAzimuth->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sailDispLayout->addWidget( sliderAzimuth, 1, 1 );
        ypos++;
    }
    else
    {
        sliderElevation = NULL;
        sliderAzimuth = NULL;
    }

    /* create the drawing area */
    switch (viewMode)
    {
    case SHADED:
        lblDraw = new CSailDispGL( this );
        sailDispLayout->addWidget( (CSailDispGL*)(lblDraw), 0, xpos );
        break;

    case WIREFRAME:
    default:
        lblDraw = new CSailDispLabel( this );
        sailDispLayout->addWidget( (CSailDispLabel*)(lblDraw), 0, xpos );
        break;
    }

    //((QWidget*)lblDraw)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // signals and slots connections
    if (sliderElevation)
        connect( sliderElevation, SIGNAL( valueChanged(int) ), this, SLOT( slotElevation(int) ) );
    if (sliderAzimuth)
        connect( sliderAzimuth, SIGNAL( valueChanged(int) ), this, SLOT( slotAzimuth(int) ) );
}


/** We received a keypress. This is used to zoom in and out.
 */
void CSailViewer::keyPressEvent ( QKeyEvent * e )
{
    // zoom in/out by keyboard shortcut
    if (e->modifiers() & Qt::ControlModifier)
    {
        if (e->text()=="+")
            slotZoomIn();

        if (e->text()=="=")
            slotZoomIn();

        if (e->text()=="-")
            slotZoomOut();
    }
}


/** Changes the displayed object.
 *
 * @param obj the new object to be displayed
 */
void CSailViewer::setObject(const CPanelGroup &obj)
{
    lblDraw->setObject(obj);
    lblDraw->redraw();
}


void CSailViewer::slotAzimuth(int azimuth)
{
    azimuthChanged(azimuth);
    lblDraw->setAzimuth(azimuth);
    lblDraw->redraw();
}


void CSailViewer::slotElevation(int elevation)
{
    elevationChanged(elevation);
    lblDraw->setElevation(elevation);
    lblDraw->redraw();
}


/** Draws the panel labels
 */
void CSailViewer::slotLabeling()
{
    // invert the drawLabels flag, then ask for a redraw
    lblDraw->drawLabels = ! lblDraw->drawLabels;
    lblDraw->redraw();
}


/** Resets the angle and center of the CSailDisp to their default values.
 */
void CSailViewer::slotResetView()
{
    if (sliderElevation)
        sliderElevation->setValue(0);
    if (sliderAzimuth)
        sliderAzimuth->setValue(0);
    lblDraw->resetZoomCenter();
    lblDraw->redraw();
}


/** This event occurs when the user presses the "zoom in" button.
 */
void CSailViewer::slotZoomIn()
{
    lblDraw->zoomIn();
    lblDraw->redraw();
}


/** This event occurs when the user presses the "zoom out" button.
 */
void CSailViewer::slotZoomOut()
{
    lblDraw->zoomOut();
    lblDraw->redraw();
}
