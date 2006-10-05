/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: widgetprofilevert.h,v 1.7 2006/01/25 21:54:02 jeremy_laine Exp $
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

#ifndef WIDGETPROFILEVERT_H
#define WIDGETPROFILEVERT_H

#include <QWidget>
#include <QLabel>
#include "disparea.h"

class QLabel;
class QSpinBox;
class QGroupBox;
class QSlider;
class QResizeEvent;

class CSailMould;
class CLabelProfileVert;
class CRect3d;


/** Drawing area for the vertical repartition.
 */
class CLabelProfileVert : public QLabel, CDispArea
{
public:
    CLabelProfileVert(QWidget *, CSailMould *);

protected:
    void paintEvent( QPaintEvent *);
    void resizeEvent( QResizeEvent * );

    /** the CSailMould we are displaying */
    CSailMould *mould;

    /** the logical viewport */
    CRect3d m_lRect;
};


/** Widget for displaying vertical repartition.
 *
 * @see CProfile
 * @see CLabelProfile
 * @see CLabelProfileVert
 */
class CWidgetProfileVert : public QWidget
{
    Q_OBJECT

public:
    CWidgetProfileVert( QWidget*, CSailMould*, QString );

    /** redraw the vertical repartion drawing */
    void updateLabel()
    {
        lblDrawVert->update();
    }

protected slots:
    virtual void slotSliderVert();

    // member variables
protected:
    /** the CSailMould */
    CSailMould *sailmould;

    /** groupbox for vertical repartition */
    QGroupBox* grpVertical;
    /** label for the vertical repartition */
    QLabel* lblVertical;
    /** slider for the vertical repartition */
    QSlider* sliderVertical;
    /** the vertical repartition drawing area */
    CLabelProfileVert * lblDrawVert;


};

#endif // WIDGETPROFILEVERT_H
