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

#ifndef SAILVIEWER_H
#define SAILVIEWER_H

#include <QWidget>
#include "sailcpp/panelgroup.h"

class QSlider;
class QLabel;
class CPanelGroup;
class CSailDisp;
class QGridLayout;

// enumerated types
enum enumViewMode { WIREFRAME, SHADED };


/** This class is used to display a sail. It has a display
 *  area as well as an elevation and an azimuth slider.
 */
class CSailViewer : public QWidget
{
    Q_OBJECT
public:
    CSailViewer(QWidget *parent, enumViewMode viewMode, bool show_sliders = true);

    void setObject( const CPanelGroup &obj );
    void keyPressEvent ( QKeyEvent * e );

protected slots:
    void slotLabeling();
    void slotSlider();
    void slotZoomIn();
    void slotZoomOut();

public slots:
    void slotResetView();

signals:
    /** Signals that the azimuth has changed. */
    void azimuthChanged(real azimuth);
    /** Signals that the elevation has changed. */
    void elevationChanged(real elevation);

protected:
    /** The drawing area */
    CSailDisp *lblDraw;
    /** The slider that controls the viewing elevation */
    QSlider *sliderElevation;
    /** The slider that controls the viewing azimuth */
    QSlider *sliderAzimuth;

    /** The widget's layout */
    QGridLayout* sailDispLayout;
};

#endif
