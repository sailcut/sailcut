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

#include "formrig.h"
#include "sailcutqt.h"
#include "sailviewer-panel.h"
#include "rigdef-panel.h"
#include "sailworker.h"
#include "sailwriter-xml.h"

#include <QLayout>
#include <QMenuBar>
#include <QFileInfo>

/**
 * The constructor.
 *
 * @param prefs the user preferences
 */
CFormRig::CFormRig(CPrefs *myPrefs)
        : CFormDocument(myPrefs)
{
    // create main widget
    setupMainWidget();
    
    // create menu bar
    setupMenuBar();

    // set language
    languageChange();

    // resize to prefered size
    resize( QSize(prefs->rigWindowWidth, prefs->rigWindowHeight).expandedTo(minimumSizeHint()) );

    // connect signals to slots
    connect(defpanel, SIGNAL(signalUpdate(const CRigDef& )), this, SLOT(slotUpdate(const CRigDef& )));
}


/**
 * This event is received when the user closes the dialog.
 *
 * @param e the QCloseEvent
 */
void CFormRig::closeEvent(QCloseEvent *e)
{
    prefs->rigWindowHeight = height();
    prefs->rigWindowWidth = width();
    QMainWindow::closeEvent(e);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormRig::languageChange()
{
    setWindowTitle( tr("Rig") );

    /* File menu */
    menuFile->setTitle( tr("&File") );
    actionNew->setText( tr("&New") );
    menuAdd->setTitle( tr("&Add sail") );
    actionAddSailDef->setText( tr("sail &definition") );
    actionAddSail->setText( tr("3D &sail") );

    actionOpen->setText( tr("&Open") );

    defpanel->languageChange();
    viewer->languageChange();

}


/**
 * We got a new sail definition, update widgets.
 *
 * @param newdef The new rig definition
 */
void CFormRig::setRigDef(const CRigDef &newdef)
{
    rigdef = newdef;
    viewer->setObject(rigdef.makeViewSail());
    defpanel->setRigDef(rigdef);
}


/**
 * Creates the main widget
 */
void CFormRig::setupMainWidget()
{
    QWidget *mainwidget = new QWidget(this);
    viewer = new CSailViewerPanel(mainwidget, WIREFRAME, true, false);
    defpanel = new CRigDefPanel(mainwidget);

    QGridLayout *layout = new QGridLayout( mainwidget );
    layout->addWidget(viewer, 0, 0);
    layout->setRowStretch(0, 2);
    layout->addWidget(defpanel, 1, 0);
    layout->setRowStretch(1, 1);

    setCentralWidget(mainwidget);
}


/**
 * Creates the menu bar
 */
void CFormRig::setupMenuBar()
{
    /* create blank menu bar */
    menuFile = menuBar()->addMenu( "" );

    actionNew = menuFile->addAction( "", this, SLOT( slotNew() ) );

    menuAdd = menuFile->addMenu( "" );
    actionAddSailDef = menuAdd->addAction( "", this, SLOT( slotAddSailDef() ) );
    actionAddSail = menuAdd->addAction( tr("3D &sail"), this, SLOT( slotAddSail() ) );

    actionOpen = menuFile->addAction( "", this, SLOT( slotOpen() ) );
}


/**
 * The file menu's "Add->3d sail item" was clicked.
 */
void CFormRig::slotAddSail()
{
    CRigSail rigsail;
    QString newname = CPanelGroupXmlWriter().readDialog((CPanelGroup&)rigsail,"");

    if (!newname.isNull())
    {
        rigsail.type = SAIL3D;
        rigsail.filename = newname;
        rigdef.rigsail.push_back(rigsail);
        setRigDef(rigdef);
    }

}


/**
 * The file menu's "Add->Sail definition" item was clicked.
 */
void CFormRig::slotAddSailDef()
{
    CSailDef saildef;
    QString newname = CSailDefXmlWriter().readDialog(saildef,"");

    if (!newname.isNull())
    {
        CRigSail rigsail;
        (CPanelGroup&)rigsail = CSailWorker(saildef).makeSail();
        rigsail.type = SAILDEF;
        rigsail.filename = newname;
        rigdef.rigsail.push_back(rigsail);
        setRigDef(rigdef);
    }

}


/**
 * The file menu's New item was clicked
 */
void CFormRig::slotNew()
{
    setRigDef(CRigDef());
}


/**
 * The file menu's Open item was clicked.
 */
void CFormRig::slotOpen()
{
    CRigDef newdef;
    QString newname = CRigDefXmlWriter().readDialog(newdef,filename);

    if (!newname.isNull())
    {
        filename = newname;
        setRigDef(newdef);
        prefs->mruRigdef.touchEntry(filename);
    }
}


/**
 * The file menu's Save item was clicked.
 */
bool CFormRig::save()
{
    if ( filename.isEmpty() )
    {
        return saveAs();
    }

    // try writing to file, catch exception
    try
    {
        CRigDefXmlWriter().write(rigdef, filename);
        prefs->mruRigdef.touchEntry(filename);
        return true;
    }
    catch (CException e)
    {
        QMessageBox::information(this, tr("error"), tr("There was an error writing to the selected file"));
    }
    return false;
}


/**
 * The file menu's Save As item was clicked.
 */
bool CFormRig::saveAs()
{
    QString newname = CRigDefXmlWriter().writeDialog(rigdef, filename);

    if (!newname.isNull())
    {
        filename = newname;
        prefs->mruRigdef.touchEntry(filename);
        return true;
    }
    return false;
}


/**
 * The rig definition was modified by the user.
 *
 * @param newdef The new rig definition
 */
void CFormRig::slotUpdate(const CRigDef& newdef)
{
    rigdef = newdef;
    viewer->setObject(rigdef.makeViewSail());
}
