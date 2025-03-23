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

#ifndef BOATDEF_PANEL_H
#define BOATDEF_PANEL_H

#include "sailcpp/boatdef.h"

#include <QWidget>

class QGroupBox;
class QTabWidget;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;


/**
 * A widget for displaying info about a CVector3d.
 */
class CVector3dWidget : public QWidget
{
    Q_OBJECT

public:
    CVector3dWidget(QWidget *parent);

    CVector3d getVector();
    void setVector(const CVector3d &v);

protected:
    /** The spin boxes. */
    std::vector<QSpinBox*> spinBox;

};

/** A widget for displaying information about a boat element.
 */
class CBoatElementWidget : public QWidget
{
    Q_OBJECT

public:
    CBoatElementWidget(QWidget *parent);

    void setElement(const CBoatElement &newelement);

signals:
    /** Signals that the user hit the "Remove" button. */
    void signalRemove();
    /** Signals that the element was modified. */
    void signalUpdate(const CBoatElement& newelement);

private slots:
    void languageChange();

protected slots:
    void slotReload();
    void slotRemove();
    void slotUpdate();

protected:
    /** the boat element we are operating on */
    CBoatElement element;

    /** groupbox for the element info */
    QGroupBox* grpInfo;

    /** label for the filename */
    QLabel* lblFile;
    /** label for the element name */
    QLineEdit* txtName;
    /** static label saying "file" */
    QLabel* lblFileStatic;
    /** static label saying "name" */
    QLabel* lblNameStatic;

    /** widget for the origin */
    CVector3dWidget *wdgOrigin;

    /** button to accept changes to the CBoatElement */
    QPushButton *btnUpdate;
    /** button to reload this element from file */
    QPushButton *btnReload;
    /** button to remove this CBoatElement */
    QPushButton *btnRemove;

};


/** A panel displaying one tab per element of a given
 *  boat definition.
 */
class CBoatDefPanel : public QWidget
{
    Q_OBJECT

public:
    CBoatDefPanel(QWidget *parent);

    void setDef(const CBoatDef &newdef);

public slots:
    virtual void slotRemove();
    virtual void slotUpdate(const CBoatElement& newelement);

signals:
    /**  Signals that the boat definition was modified. */
    void signalUpdate(const CBoatDef& newdef);

protected:
    /** the tabbed widget, with one tab per boat element */
    QTabWidget *tabs;

    /** the widgets for the boat element parameters */
    std::vector<CBoatElementWidget*> elementwidget;

    /** the boat definition we are operating one */
    CBoatDef def;
};

#endif
