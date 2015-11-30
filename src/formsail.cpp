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

#include "formprint.h"
#include "formsail.h"
#include "formsaildef.h"
#include "formmould.h"

#include "sailprinter.h"
#include "sailcpp/sailworker.h"
#include "sailwriter-carlson.h"
#include "sailwriter-dxf.h"
#include "sailwriter-hand.h"
#include "sailwriter-svg.h"
#include "sailwriter-txt.h"

#include <QLayout>
#include <QMenu>
#include <QTabWidget>
#include <QPrintDialog>
#include <QPrinter>


/**
 * Constructs a window to display a sail.
 *
 * @param parent the parent widget
 */
CFormSail::CFormSail(QWidget *parent)
    : CFormMain(parent)
{
    // create menu bar
    setupMenuBar();

    // create main widget
    setupMainWidget();

    // set language
    connect(qApp, SIGNAL(languageChanged()),
            this, SLOT(languageChange()));
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormSail::languageChange()
{
    setWindowTitle( tr("sail") );

    // print submenu
    menuPrint->setTitle( tr("&Print") );
    actionPrintData->setText( tr("data") );
    actionPrintDwg->setText( tr("drawing") );
    actionPrintDev->setText( tr("development") );

    // export 3d submenu
    menuExport3d->setTitle( tr("E&xport 3D sail") );
    actionExport3dDXF->setText( tr("to &DXF") );
    actionExport3dDXFSplit->setText( tr("to DXF (split)") );
    actionExport3dSVG->setText( tr("to &SVG") );
    actionExport3dTXT->setText( tr("to &TXT sail") );
    actionExport3dXML->setText( tr("to &XML sail") );

    // export flat submenu
    menuExportFlat->setTitle( tr("Export &development") );
    actionExportFlatCarlson->setText( tr("to &Carlson plotter") );
    actionExportFlatDXF->setText( tr("to &DXF") );
    actionExportFlatDXFSplit->setText( tr("to DXF (split)") );
    //actionExportFlatDXFBlocks->setText( tr("to &DXF-BLOCKS") );
    actionExportFlatHand->setText( tr("to &Hand-plotting format") );
    actionExportFlatSVG->setText( tr("to &SVG") );
    actionExportFlatTXT->setText( tr("to &TXT sail") );
    actionExportFlatXML->setText( tr("to &XML sail") );

    // view menu
    actionViewDef->setText( tr("&Dimensions") );
    actionViewMould->setText( tr("&Mould") );
    actionViewPatch->setText( tr("&Patches") );

    // tabs
    tabs->setTabText(tabs->panel.size()-1, tr("development"));
}


bool CFormSail::open(const QString &newfile)
{
    CSailDefXmlWriter writer;
    try {
        setDef(writer.read(newfile));
        filename = newfile;
        return true;
    } catch (read_error e) {
        writer.readErrorMessage();
    }
    return false;
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormSail::setDef(const CSailDef& newdef)
{
    def = newdef;
    sail = CSailWorker(def).makeSail(flatsail,dispsail);
    tabs->setObject(sail);
    tabs->panel[tabs->panel.size()-1]->setObject(dispsail);
}


/**
 * Creates the menu bar
 */
void CFormSail::setupMenuBar()
{
    // File sub menus //
    // print submenu
    menuPrint = new QMenu(this);
    actionPrintData = menuPrint->addAction("", this, SLOT( slotPrintData() ));
    actionPrintDwg = menuPrint->addAction("", this, SLOT( slotPrintDwg() ));
    actionPrintDev = menuPrint->addAction("", this, SLOT( slotPrintDev() ));
    addFileMenu(menuPrint);

    // export 3d submenu
    menuExport3d = new QMenu(this);
    actionExport3dDXF = menuExport3d->addAction("", this, SLOT( slotExportDXF() ) );
    actionExport3dDXFSplit = menuExport3d->addAction("", this, SLOT( slotExportDXFSplit() ) );
    actionExport3dSVG = menuExport3d->addAction("", this, SLOT( slotExportSVG() ) );
    actionExport3dTXT = menuExport3d->addAction("", this, SLOT( slotExportTXT() ) );
    actionExport3dXML = menuExport3d->addAction("", this, SLOT( slotExportXML() ) );
    addFileMenu(menuExport3d);

    // export flat submenu
    menuExportFlat = new QMenu(this); //menuFile->addMenu("");
    actionExportFlatCarlson = menuExportFlat->addAction("", this, SLOT( slotExportFlatCarlson() ) );
    actionExportFlatDXF = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXF() ) );
    actionExportFlatDXFSplit = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXFSplit() ) );
    //actionExportFlatDXFBlocks = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXFBlocks() ) );
    actionExportFlatHand = menuExportFlat->addAction("", this, SLOT( slotExportFlatHand() ) );
    actionExportFlatSVG = menuExportFlat->addAction("", this, SLOT( slotExportFlatSVG() ) );
    actionExportFlatTXT = menuExportFlat->addAction("", this, SLOT( slotExportFlatTXT() ) );
    actionExportFlatXML = menuExportFlat->addAction("", this, SLOT( slotExportFlatXML() ) );
    addFileMenu(menuExportFlat);

    // View actions

    actionViewDef = new QAction(this);
    connect( actionViewDef, SIGNAL( triggered() ), this, SLOT( slotDef() ) );
    addViewAction(actionViewDef);

    actionViewMould = new QAction(this);
    connect( actionViewMould, SIGNAL( triggered() ), this, SLOT ( slotMould() ) );
    addViewAction(actionViewMould);

    // TODO : enable the following action when the patch viewer is ready
    actionViewPatch = new QAction(this);
    actionViewPatch->setEnabled(false);
    addViewAction(actionViewPatch);
}


/**
 * Creates the main widget
 */
void CFormSail::setupMainWidget()
{
    tabs = new CSailViewerTabs(this);
    tabs->addViewer(new CSailViewerPanel(NULL, WIREFRAME, false));
    setCentralWidget(tabs);
}


/**
 * Displays the sail CFormSailDef sail definition dialog.
 */
void CFormSail::slotDef()
{
    // we pass the CFormSailDef a pointer to a copy of the sail definition so
    // that it can update it if necessary
    CSailDef defcopy = def;

    if ( CFormSailDef(this , &defcopy).exec() )
    {
        // we returned from the dialog with an 'OK',
        setDef(defcopy);
    }
}


/**
 * Export the 3D sail to a DXF file.
 */
void CFormSail::slotExportDXF()
{
    CSailDxfWriter3d(CSailDxfWriter3d::NORMAL).writeDialog(sail);
}


/**
 * Export the 3D sail to several DXF files (one per panel).
 */
void CFormSail::slotExportDXFSplit()
{
    CSailDxfWriter3d(CSailDxfWriter3d::SPLIT).writeDialog(sail);
}


/**
 * Exports the 3D sail to an SVG file.
 */
void CFormSail::slotExportSVG()
{
    CSailSvgWriter().writeDialog(sail);
}


/**
 * Exports the 3D sail to a TXT file.
 */
void CFormSail::slotExportTXT()
{
    CSailTxtWriter().writeDialog(sail);
}


/**
 * Exports the 3D sail to an XML file.
 */
void CFormSail::slotExportXML()
{
    CPanelGroupXmlWriter().writeDialog(sail);
}


/**
 * Exports the flat sail to a Carlson plotter file
 */
void CFormSail::slotExportFlatCarlson()
{
    CSailCarlsonWriter().writeDialog(flatsail);
}


/**
 * Exports the flat sail with panels staggered as displayed to a DXF file
  */
void CFormSail::slotExportFlatDXF()
{
    CSailDxfWriter2d(CSailDxfWriter2d::NORMAL).writeDialog(dispsail);
}


/**
 * Export the flat sail to several DXF files (one per panel).
 */
void CFormSail::slotExportFlatDXFSplit()
{
    CSailDxfWriter2d(CSailDxfWriter2d::SPLIT).writeDialog(flatsail);
}


/**
 * Exports the flat sail with panels superimposed to a DXF file with blocks
  */
void CFormSail::slotExportFlatDXFBlocks()
{
    // FIXME: shouldn't this be BLOCKS ?
    CSailDxfWriter2d(CSailDxfWriter2d::NORMAL).writeDialog(flatsail);
}


/**
 * Exports the flat sail to a "hand" sail file.
 */
void CFormSail::slotExportFlatHand()
{
    CSailHandWriter().writeDialog(flatsail);
}


/**
 * Exports the flat sail to an SVG file.
 */
void CFormSail::slotExportFlatSVG()
{
    CSailSvgWriter().writeDialog(dispsail);
}


/**
 * Exports the flat sail to a TXT sail file.
 */
void CFormSail::slotExportFlatTXT()
{
    CSailTxtWriter().writeDialog(flatsail);
}


/**
 * Exports the flat sail to an XML sail file
 */
void CFormSail::slotExportFlatXML()
{
    CPanelGroupXmlWriter().writeDialog(flatsail);
}


/**
 * Displays the CFormMould sail mould definition dialog.
 */
void CFormSail::slotMould()
{
    // we pass the CFormMould a pointer to a copy of the sail mould so
    // that it can update it if necessary
    CSailDef defcopy = def;

    if ( CFormMould(this , &defcopy.mould).exec() )
    {
        // we returned from the dialog with an 'OK'
        setDef(defcopy);
    }
}


/**
 * Print the current sail data.
 */
void CFormSail::slotPrintData()
{
    CSailDataPrinter prt(def);
    CFormPrint(&prt, QPrinter::Portrait).exec();
}


/**
 * Print the current developed sail drawings one panel per page
 * with coordinates of key points for handplotting.
 */
void CFormSail::slotPrintDev()
{
    CSailDevelPrinter prt(flatsail);
    CFormPrint(&prt, QPrinter::Landscape).exec();
}


/**
 * Print the current sail drawing.
 */
void CFormSail::slotPrintDwg()
{
    CSailDrawingPrinter prt(sail);
    CFormPrint(&prt, QPrinter::Portrait).exec();
}


