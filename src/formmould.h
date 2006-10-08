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

#ifndef FORMMOULD_H
#define FORMMOULD_H

#include <QDialog>

class QPushButton;
class QGroupBox;
class QLabel;
class QSlider;

class CWidgetProfile;
class CWidgetProfileVert;
class CSailMould;

/** The sail mould definition dialog.
 */
class CFormMould : public QDialog
{
public:
    CFormMould( QWidget* , CSailMould* );
    /** The destructor */
    ~CFormMould()
    {}
    ;

protected:
    /** the CSailMould */
    CSailMould *sailmould;

    /** Help button */
    QPushButton* buttonHelp;
    /** OK button */
    QPushButton* buttonOk;
    /** Cancel button */
    QPushButton* buttonCancel;

    /** the vertical repartition widget */
    CWidgetProfileVert * widgetVert;

    /** the top CWidgetProfile */
    CWidgetProfile * prfTop;
    /** the middle CWidgetProfile */
    CWidgetProfile * prfMiddle;
    /** the bottom CWidgetProfile */
    CWidgetProfile * prfBottom;

};


#endif // FORMMOULD_H