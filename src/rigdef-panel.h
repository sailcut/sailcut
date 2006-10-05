/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: rigdef-panel.h,v 1.8 2006/01/25 21:54:00 jeremy_laine Exp $
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

#ifndef RIGDEF_PANEL_H
#define RIGDEF_PANEL_H

#include "rigdef.h"

#include <QWidget>

class QGroupBox;
class QTabWidget;
class QLabel;
class QLineEdit;
class QPushButton;


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
    /** The text boxes. */
    vector<QLineEdit *>txtBox;
};

/** A widget for displaying information about a rigsail.
 */
class CRigSailWidget : public QWidget
{
    Q_OBJECT

public:
    CRigSailWidget(QWidget *parent);

    void setRigSail(const CRigSail &newsail);

public slots:
    virtual void languageChange();

signals:
    /** Signals that the user hit the "Remove" button. */
    void signalRemove();
    /** Signals that the rigsail was modified. */
    void signalUpdate(const CRigSail& newsail);

protected slots:
    void slotReload();
    void slotRemove();
    void slotUpdate();

protected:
    /** the rigsail we are operating on */
    CRigSail rigsail;

    /** groupbox for the sail info */
    QGroupBox* grpInfo;

    /** label for the filename */
    QLabel* lblFile;
    /** label for the sail name */
    QLineEdit* txtName;
    /** static label saying "file" */
    QLabel* lblFileStatic;
    /** static label saying "name" */
    QLabel* lblNameStatic;

    /** widget for the origin */
    CVector3dWidget *wdgOrigin;

    /** button to accept changes to the CRigSail */
    QPushButton *btnUpdate;
    /** button to reload this sail from file */
    QPushButton *btnReload;
    /** button to remove this CRigSail */
    QPushButton *btnRemove;

};


/** A panel displaying one tab per rigsail of a given
 *  rig definition.
 */
class CRigDefPanel : public QWidget
{
    Q_OBJECT

public:
    CRigDefPanel(QWidget *parent);

    void setRigDef(const CRigDef &newdef);

public slots:
    virtual void languageChange();
    virtual void slotRemove();
    virtual void slotUpdate(const CRigSail& newsail);

signals:
    /**  Signals that the rig definition was modified. */
    void signalUpdate(const CRigDef& newdef);

protected:
    /** the tabbed widget, with one tab per rigsail */
    QTabWidget *tabs;

    /** the widgets for the rigsail parameters */
    vector <CRigSailWidget*> sailwidget;

    /** the rig definition we are operating one */
    CRigDef rigdef;
};

#endif
