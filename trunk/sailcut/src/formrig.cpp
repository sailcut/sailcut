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
#include "sailreader-xml.h"
#include "sailwriter-xml.h"

#include <QLayout>
#include <QMenuBar>
#include <QFileInfo>

/**
 * The constructor.
 *
 * @param myApp the Sailcut application
 * @param parent the parent widget
 */
CFormRig::CFormRig(CSailApp *myApp, QWidget *parent)
        : QMainWindow(parent), prefs(&myApp->prefs)
{
    // create menu bar
    setupMenuBar();

    // create main widget
    setupMainWidget();

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
    setWindowTitle( "Sailcut CAD - " + tr("Rig") );

    /* File menu */
    menuFile->setTitle( tr("&File") );
    actionNew->setText( tr("&New") );
    menuAdd->setTitle( tr("&Add sail") );
    actionAddSailDef->setText( tr("sail &definition") );
    actionAddSail->setText( tr("3D &sail") );

    actionOpen->setText( tr("&Open") );
    menuRecent->setTitle( tr("Open &recent") );
    actionSave->setText( tr("&Save") );
    actionSaveAs->setText( tr("Save &As") );
    actionClose->setText( tr("&Close") );

    defpanel->languageChange();
    viewer->languageChange();

}


/**
 * Creates the "Open Recent" menu from the Most Recently Used files list.
 */
void CFormRig::makeMenuMru()
{
    menuRecent->clear();
    for ( unsigned int i = 0; i < prefs->mruRigdef.size(); i++)
    {
        menuRecent->addAction( prefs->mruRigdef[i], this, SLOT ( slotOpenRecent() ))->setData( i );
    }
}


/**
 * We got a new sail definition, update widgets.
 *
 * @param newdef The new rig definition
 */
void CFormRig::setRigDef(const CRigDef &newdef)
{
    rigdef = newdef;
    viewer->setObjects(rigdef.makeViewSail());
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
    menuRecent = menuFile->addMenu( "" );
    menuFile->addSeparator();
    actionSave = menuFile->addAction( "", this, SLOT( slotSave() ) );
    actionSaveAs = menuFile->addAction( "", this, SLOT( slotSaveAs() ) );
    menuFile->addSeparator();
    actionClose = menuFile->addAction( "", this, SLOT( close() ) );

    makeMenuMru();
}


/**
 * The file menu's "Add->3d sail item" was clicked.
 */
void CFormRig::slotAddSail()
{
    CRigSail rigsail;
    QString newname = CSailXmlReader("sail").readDialog((CSail&)rigsail,"");

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
    QString newname = CSailDefXmlReader("saildef").readDialog(saildef,"");

    if (!newname.isNull())
    {
        CRigSail rigsail;
        (CSail&)rigsail = CSailWorker(saildef).makeSail();
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
    QString newname = CRigDefXmlReader("rigdef").readDialog(newdef,filename);

    if (!newname.isNull())
    {
        filename = newname;
        setRigDef(newdef);
        prefs->mruRigdef.touchEntry(filename);
        makeMenuMru();
    }
}


/**
 * Opens a recently used rig definition.
 */
void CFormRig::slotOpenRecent()
{
    // retrieve the index of the MRU entry
    QAction *a = qobject_cast<QAction *>(sender());
    if (!a)
        return;
    int index = a->data().toInt();

    filename = prefs->mruRigdef[index];
    try
    {
        setRigDef(CRigDefXmlReader("rigdef").read(filename));
        prefs->mruRigdef.touchEntry(filename);
    }
    catch (CException e)
    {
        prefs->mruRigdef.removeEntry(filename);
    }
    makeMenuMru();

}


/**
 * The file menu's Save item was clicked.
 */
void CFormRig::slotSave()
{
    if ( filename.isEmpty() )
    {
        slotSaveAs();
        return;
    }

    // try writing to file, catch exception
    try
    {
        CRigDefXmlWriter(rigdef,"rigdef").write(filename);
        prefs->mruRigdef.touchEntry(filename);
        makeMenuMru();
    }
    catch (CException e)
    {
        QMessageBox::information(this, tr("error"), tr("There was an error writing to the selected file"));
    }
}


/**
 * The file menu's Save As item was clicked.
 */
void CFormRig::slotSaveAs()
{
    QString newname = CRigDefXmlWriter(rigdef,"rigdef").writeDialog(filename);

    if (!newname.isNull())
    {
        filename = newname;
        prefs->mruRigdef.touchEntry(filename);
        makeMenuMru();
    }
}


/**
 * The rig definition was modified by the user.
 *
 * @param newdef The new rig definition
 */
void CFormRig::slotUpdate(const CRigDef& newdef)
{
    rigdef = newdef;
    viewer->setObjects(rigdef.makeViewSail());
}
