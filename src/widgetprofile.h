/*
 * Copyright (C) 1993-2009 Robert & Jeremy Laine
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

#ifndef WIDGETPROFILE_H
#define WIDGETPROFILE_H

#include <QLabel>
#include "sailcpp/sailcalc.h"

class QSpinBox;

class CProfile;
class CWidgetProfileVert;

/**************************************************************************

                         CLabelProfile class

**************************************************************************/

/** Drawing area fpr a CProfile.
 */
class CLabelProfile : public QLabel
{
public:
    CLabelProfile(QWidget *, CProfile *);

protected:
    void paintEvent( QPaintEvent *);
    void resizeEvent( QResizeEvent * );

private:
    /** the CProfile we are displaying */
    CProfile* profile;

    /** the logical viewport rectangle */
    CRect3d lRect;

    /** Has the area been resized since last redraw */
    bool wasResized;
};


/**************************************************************************

                         CWidgetProfile class

**************************************************************************/

/** Widget for displaying a CProfile. It has a display area and 3 spinboxes.
 *
 * @see CProfile
 * @see CLabelProfile
 * @see CWidgetProfileVert
 */
class CWidgetProfile : public QWidget
{
    Q_OBJECT

public:
    CWidgetProfile( QWidget*, CProfile*, QString, CWidgetProfileVert* );

protected slots:
    virtual void languageChange();
    void slotChanged();

    // member variables
public:
    /** label for luff */
    QLabel* lblLuff;
    /** label for depth */
    QLabel* lblDepth;
    /** label for percent */
    QLabel* lblPercent;
    /** label for leech */
    QLabel* lblLeech;

    /** label for Luff slope value */
    QLabel* lblLuffSlopeValue;
    /** label for Luff slope 'degrees' */
    QLabel* lblLuffSlopeDegrees;
    /** label for max depth position */
    QLabel* lblMaxPosValue;
    /** label for max depth 'cord' */
    QLabel* lblMaxPosCord;
    /** label for leech slope value */
    QLabel* lblLeechSlopeValue;
    /** label for leech slope 'degrees' */
    QLabel* lblLeechSlopeDegrees;

    /** spinbox for luff */
    QSpinBox* spinLuff;
    /** spinbox for depth */
    QSpinBox* spinDepth;
    /** spinbox for leech */
    QSpinBox* spinLeech;

protected:
    /** drawing area for the profile */
    CLabelProfile* lblDraw;
    /** widget for the vertical repartition */
    CWidgetProfileVert *widgetVert;

    /** the profile we are displaying */
    CProfile* profile;
    /** active flag to disable slotChanged */
    bool active;
};

#endif // WIDGETPROFILE_H
