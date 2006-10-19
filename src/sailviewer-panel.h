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

#ifndef SAILVIEWER_PANEL_H
#define SAILVIEWER_PANEL_H

#include "sailviewer.h"

// forward definitions
class QGroupBox;
class QLabel;
class QPushButton;


/** The main dialog of the Sailcut application.
 */
class CSailViewerPanel : public QWidget
{
    Q_OBJECT

public:
    CSailViewerPanel(QWidget *parent, const enumViewMode viewMode, bool show_sliders, bool show_labeling = true);

    void setObject(const CPanelGroup &obj);
    void keyPressEvent ( QKeyEvent * e );

    // slots
public slots:
    virtual void languageChange();
    virtual void slotAzimuth(real azimuth);
    virtual void slotElevation(real azimuth);

    // member variables
protected:
    /** groupbox for parameters */
    QGroupBox* grpParams;
    /** label for current azimuth */
    QLabel* lblAzimuth;
    /** label for current elevation */
    QLabel* lblElevation;
    /** static label saying "elevation" */
    QLabel* lblElevationStatic;
    /** static label saying "azimuth" */
    QLabel* lblAzimuthStatic;
    /** groupbox for the view controls */
    QGroupBox* grpControls;
    /** Reset View button */
    QPushButton* btnResetView;
    /** Labeling button */
    QPushButton* btnLabeling;
    /** Zoom In button */
    QPushButton* btnZoomIn;
    /** Zoom Out button */
    QPushButton* btnZoomOut;

    /** display area for the 3d sail */
    CSailViewer *sailDisp;

    /** the layout for the form */
    //QGridLayout *layout;

private:
    bool showSliders;
};

#endif
