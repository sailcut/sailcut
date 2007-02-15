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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "formsail.h"
#include "formsaildef.h"
#include "formmould.h"

#include "sailprinter.h"
#include "sailworker.h"
#include "sailwriter-carlson.h"
#include "sailwriter-dxf.h"
#include "sailwriter-hand.h"
#include "sailwriter-txt.h"
#include "sailviewer-panel.h"

#include <QLayout>
#include <QMenu>
#include <QTabWidget>
#include <QPrintDialog>
#include <QPrinter>


/**
 * Constructs a window to display a sail.
 *
 * @param myPrefs the user preferences
 * @param parent the parent widget
 */
CFormSail::CFormSail(CPrefs *myPrefs, QWidget *parent)
        : CFormDocumentTmpl<CSailDef, CSailDefXmlWriter> (myPrefs, parent)
{
    setMinimumSize( QSize( 300, 220 ) );

    // create menu bar
    setupMenuBar();

    // create main widget
    setupMainWidget();

    // set language
    languageChange();

    // set initial definition
    setDef(def);
}


/**
 * We received a keypress, we pass it down to the visible tab.
 */
void CFormSail::keyPressEvent ( QKeyEvent * e )
{
    panel[tabs->currentIndex()]->keyPressEvent(e);
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormSail::languageChange()
{
    int tabidx = 0;
    setWindowTitle( tr("sail") );

    // print submenu
    menuPrint->setTitle( tr("&Print") );
    actionPrintData->setText( tr("data") );
    actionPrintDwg->setText( tr("drawing") );
    actionPrintDev->setText( tr("development") );

    // export 3d submenu
    menuExport3d->setTitle( tr("E&xport 3D sail") );
    actionExport3dDXF->setText( tr("to &DXF") );
    actionExport3dTXT->setText( tr("to &TXT sail") );
    actionExport3dXML->setText( tr("to &XML sail") );

    // export flat submenu
    menuExportFlat->setTitle( tr("Export &development") );
    actionExportFlatCarlson->setText( tr("to &Carlson plotter") );
    actionExportFlatDXF->setText( tr("to &DXF") );
    //actionExportFlatDXFBlocks->setText( tr("to &DXF-BLOCKS") );
    actionExportFlatHand->setText( tr("to &Hand-plotting format") );
    actionExportFlatTXT->setText( tr("to &TXT sail") );
    actionExportFlatXML->setText( tr("to &XML sail") );


    // View menu

    actionViewDef->setText( tr("&Dimensions") );
    actionViewMould->setText( tr("&Mould") );
    actionViewPatch->setText( tr("&Patches") );

    
    // send changeLanguage to the tabs
    for (unsigned int i = 0; i < panel.size(); i++)
        panel[i]->languageChange();

#ifdef HAVE_QTOPENGL
    tabs->setTabText(tabidx++, tr("shaded view"));
#endif
    tabs->setTabText(tabidx++, tr("wireframe view"));
    tabs->setTabText(tabidx++, tr("development"));
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormSail::setDef(const CSailDef& newdef)
{
    int tabidx = 0;
    def = newdef;
    sail = CSailWorker(def).makeSail(flatsail,dispsail);

#ifdef HAVE_QTOPENGL
    panel[tabidx++]->setObject(sail);
#endif
    panel[tabidx++]->setObject(sail);
    panel[tabidx++]->setObject(dispsail);
}


/**
 * Creates the menu bar
 */
void CFormSail::setupMenuBar()
{
    // File sub menus
    
    // print submenu
    menuPrint = new QMenu(this);
    actionPrintData = menuPrint->addAction("", this, SLOT( slotPrintData() ));
    actionPrintDwg = menuPrint->addAction("", this, SLOT( slotPrintDwg() ));
    actionPrintDev = menuPrint->addAction("", this, SLOT( slotPrintDev() ));
    extraFileMenus.push_back(menuPrint);

    // export 3d submenu
    menuExport3d = new QMenu(this);
    actionExport3dDXF = menuExport3d->addAction("", this, SLOT( slotExportDXF() ) );
    actionExport3dTXT = menuExport3d->addAction("", this, SLOT( slotExportTXT() ) );
    actionExport3dXML = menuExport3d->addAction("", this, SLOT( slotExportXML() ) );
    extraFileMenus.push_back(menuExport3d);

    // export flat submenu
    menuExportFlat = new QMenu(this); //menuFile->addMenu("");
    actionExportFlatCarlson = menuExportFlat->addAction("", this, SLOT( slotExportFlatCarlson() ) );
    actionExportFlatDXF = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXF() ) );
    //actionExportFlatDXFBlocks = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXFBlocks() ) );
    actionExportFlatHand = menuExportFlat->addAction("", this, SLOT( slotExportFlatHand() ) );
    actionExportFlatTXT = menuExportFlat->addAction("", this, SLOT( slotExportFlatTXT() ) );
    actionExportFlatXML = menuExportFlat->addAction("", this, SLOT( slotExportFlatXML() ) );
    extraFileMenus.push_back(menuExportFlat);

    // View actions

    actionViewDef = new QAction(this);
    connect( actionViewDef, SIGNAL( triggered() ), this, SLOT( slotDef() ) );
    extraViewActions.push_back(actionViewDef);
    
    actionViewMould = new QAction(this);
    connect( actionViewMould, SIGNAL( triggered() ), this, SLOT ( slotMould() ) );
    extraViewActions.push_back(actionViewMould);
    
    // TODO : enable the following action when the patch viewer is ready
    actionViewPatch = new QAction(this);
    actionViewPatch->setEnabled(false);
    extraViewActions.push_back(actionViewPatch);
}


/**
 * Creates the main widget
 */
void CFormSail::setupMainWidget()
{
    // create viewers
    CSailViewerPanel *tmp;
#ifdef HAVE_QTOPENGL
    tmp = new CSailViewerPanel(0, SHADED, true);
    panel.push_back(tmp);
#endif
    tmp = new CSailViewerPanel(0, WIREFRAME, true);
    panel.push_back(tmp);
    tmp = new CSailViewerPanel(0, WIREFRAME, false);
    panel.push_back(tmp);

    // create tabs
    QGridLayout *layout = new QGridLayout(this);
    tabs = new QTabWidget(this);
    layout->addWidget(tabs);
    for (unsigned int i = 0 ; i < panel.size(); i++)
        tabs->addTab(panel[i],"");
}


/**
 * Displays the sail CFormSailDef sail definition dialog.
 */
void CFormSail::slotDef()
{  // we pass the CFormSailDef a pointer to a copy of the sail definition so
    // that it can update it if necessary
    CSailDef defcopy = def;

    if ( CFormSailDef(this , &defcopy).exec() )
    {
        // we returned from the dialog with an 'OK',
        setDef(defcopy);
    }
}


/**
 * Export the 3D sail to a DXF file
 */
void CFormSail::slotExportDXF()
{
    CSailDxfWriter3d().writeDialog(sail);
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
    CSailDxfWriter2d().writeDialog(dispsail);
}


/**
 * Exports the flat sail with panels superimposed to a DXF file with blocks
  */
void CFormSail::slotExportFlatDXFBlocks()
{
    CSailDxfWriter2d().writeDialog(flatsail);
}


/**
 * Exports the flat sail to a "hand" sail file.
 */
void CFormSail::slotExportFlatHand()
{
    CSailHandWriter().writeDialog(flatsail);
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
{  // we pass the CFormMould a pointer to a copy of the sail mould so
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
{  // try printing
    try
    {
        QPrinter myprinter;
        myprinter.setOrientation(QPrinter::Portrait);
        myprinter.setFullPage(FALSE);

        QPrintDialog printDialog(&myprinter, this);
        if ( printDialog.exec() == QDialog::Accepted )
        {
            CSailPrinter p(&myprinter);
            p.printSailData(def);
        }
    }
    catch (CException e)
    {
        QMessageBox::information(this, tr("error"), tr("There was a data printing error"));
    }
}


/**
 * Print the current developed sail drawings one panel per page
 * with coordinates of key points for handplotting.
 */
void CFormSail::slotPrintDev()
{
    try
    {  // try printing
        QPrinter myprinter;
        // set landscape printing
        myprinter.setOrientation(QPrinter::Landscape);
        myprinter.setFullPage(FALSE);

        QPrintDialog printDialog(&myprinter, this);
        if ( printDialog.exec() == QDialog::Accepted )
        {
            CSailPrinter p(&myprinter);
            p.printSailDevel(flatsail);
        }
    }
    catch (CException e)
    {
        QMessageBox::information(this, tr("error"), tr("There was a development printing error"));
    }
}


/**
 * Print the current sail drawing.
 */
void CFormSail::slotPrintDwg()
{  // try printing
    try
    {
        QPrinter myprinter;
        // set landscape printing
        myprinter.setOrientation(QPrinter::Portrait);
        myprinter.setFullPage(FALSE);

        QPrintDialog printDialog(&myprinter, this);
        if ( printDialog.exec() == QDialog::Accepted )
        {
            CSailPrinter p(&myprinter);
            p.printSailDrawing(sail);
        }
    }
    catch (CException e)
    {
        QMessageBox::information(this, tr("error"), tr("There was a drawing printing error"));
    }
}


