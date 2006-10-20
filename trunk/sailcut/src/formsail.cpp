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

#include "hullworker.h"
#include "sailcutqt.h"
#include "saildisp.h"
#include "saildoc.h"
#include "sailpainter.h"
#include "sailprinter.h"
#include "sailreader-xml.h"
#include "sailworker.h"
#include "sailwriter-carlson.h"
#include "sailwriter-dxf.h"
#include "sailwriter-hand.h"
#include "sailwriter-txt.h"
#include "sailwriter-xml.h"
#include "sailviewer-panel.h"

#include <QDir>
#include <QLayout>
#include <QMenuBar>
#include <QTabWidget>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>


/**
 * Constructs Sailcut CAD's main window.
 *
 * @param parent the parent widget
 */
CFormSail::CFormSail(CPrefs *myPrefs)
        : CFormDocument(myPrefs)
{
    setMinimumSize( QSize( 300, 220 ) );

    // create menu bar
    setupMenuBar();

    // create main widget
    setupMainWidget();

    // set language
    languageChange();

    // resize to prefered size
    resize( QSize(prefs->sailWindowWidth,prefs->sailWindowHeight).expandedTo(minimumSizeHint()) );
}


/**
 * This event is received when the user closes the dialog.
 */
void CFormSail::closeEvent(QCloseEvent *e)
{
    prefs->sailWindowHeight = height();
    prefs->sailWindowWidth = width();
    QMainWindow::closeEvent(e);
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
    setWindowTitle( "Sail" );

    menuFile->setTitle( tr("&File") );

    actionOpen->setText( tr("&Open") );

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

    menuView->setTitle( tr("&View") );
    actionViewDef->setText( tr("&Dimensions") );
    actionViewMould->setText( tr("&Mould") );
    actionViewPatch->setText( tr("&Patches") );

    
    // send changeLanguage to the tabs
    for (unsigned int i = 0; i < panel.size(); i++)
        panel[i]->languageChange();

#ifdef HAVE_GL

    tabs->setTabText(0, tr("shaded view"));
    tabs->setTabText(1, tr("wireframe view"));
    tabs->setTabText(2, tr("development"));
#else

    tabs->setTabText(0, tr("wireframe view"));
    tabs->setTabText(1, tr("development"));
#endif

}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormSail::setSailDef(const CSailDef newdef)
{
    saildef = newdef;
    sail = CSailWorker(saildef).makeSail(flatsail,dispsail);

    CPanelGroup obj_3d, obj_flat;
    obj_3d.child.push_back(sail);
    obj_flat.child.push_back(dispsail);

    // generate the hull
    if (saildef.sailType != WING)
    {
        CPanelGroup hull = CHullWorker(hulldef).makeHull();
        obj_3d.child.push_back(hull);
    }

    panel[0]->setObject(obj_3d);
#ifdef HAVE_GL
    panel[1]->setObject(obj_3d);
    panel[2]->setObject(obj_flat);
#else
    panel[1]->setObject(obj_flat);
#endif
}


/**
 * Creates the menu bar
 */
void CFormSail::setupMenuBar()
{
    // File menu
    menuFile = menuBar()->addMenu("");
    actionOpen = menuFile->addAction("", this, SLOT( slotOpen() ) );

    menuFile->addSeparator();

    // print submenu
    menuPrint = menuFile->addMenu("");
    actionPrintData = menuPrint->addAction("", this, SLOT( slotPrintData() ));
    actionPrintDwg = menuPrint->addAction("", this, SLOT( slotPrintDwg() ));
    actionPrintDev = menuPrint->addAction("", this, SLOT( slotPrintDev() ));

    menuFile->addSeparator();


    // export 3d submenu
    menuExport3d = menuFile->addMenu("");
    actionExport3dDXF = menuExport3d->addAction("", this, SLOT( slotExportDXF() ) );
    actionExport3dTXT = menuExport3d->addAction("", this, SLOT( slotExportTXT() ) );
    actionExport3dXML = menuExport3d->addAction("", this, SLOT( slotExportXML() ) );

    // export flat submenu
    menuExportFlat = menuFile->addMenu("");
    actionExportFlatCarlson = menuExportFlat->addAction("", this, SLOT( slotExportFlatCarlson() ) );
    actionExportFlatDXF = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXF() ) );
    //actionExportFlatDXFBlocks = menuExportFlat->addAction("", this, SLOT( slotExportFlatDXFBlocks() ) );
    actionExportFlatHand = menuExportFlat->addAction("", this, SLOT( slotExportFlatHand() ) );
    actionExportFlatTXT = menuExportFlat->addAction("", this, SLOT( slotExportFlatTXT() ) );
    actionExportFlatXML = menuExportFlat->addAction("", this, SLOT( slotExportFlatXML() ) );

    // View menu

    menuView = menuBar()->addMenu("");
    actionViewDef = menuView->addAction( "", this, SLOT( slotDef() ) );
    actionViewMould = menuView->addAction( "", this, SLOT ( slotMould() ) );
    // TODO : enable the following action when the patch viewer is ready
    actionViewPatch = menuView->addAction( "" );
    actionViewPatch->setEnabled(false);
}


/**
 * Creates the main widget
 */
void CFormSail::setupMainWidget()
{
    tabs = new QTabWidget(this);

    CSailViewerPanel *tmp;

#ifdef HAVE_GL

    tmp = new CSailViewerPanel(0, SHADED, true);
    panel.push_back(tmp);
#endif

    tmp = new CSailViewerPanel(0, WIREFRAME, true);
    panel.push_back(tmp);
    tmp = new CSailViewerPanel(0, WIREFRAME, false);
    panel.push_back(tmp);

    for (unsigned int i = 0 ; i < panel.size(); i++)
    {
        tabs->addTab(panel[i],"");
    }

    setCentralWidget(tabs);
}


/**
 * Opens a given sail file
 */
bool CFormSail::show(const QString newname)
{
    bool ret = true;

    // load or create sail
    CSailDef newdef;
    filename = newname;
    if ( !filename.isNull() )
    {
        try
        {
            newdef = CSailDefXmlReader().read(filename);
        } catch (CException e) {
            ret = false;
        }
    }
    setSailDef(newdef);
    QMainWindow::show();
    return ret;
}


/**
 * Displays the sail CFormSailDef sail definition dialog.
 */
void CFormSail::slotDef()
{  // we pass the CFormSailDef a pointer to a copy of the sail definition so
    // that it can update it if necessary
    CSailDef saildefcopy = saildef;

    if ( CFormSailDef(this , &saildefcopy).exec() )
    {
        // we returned from the dialog with an 'OK',
        setSailDef(saildefcopy);
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
    CSailDef saildefcopy = saildef;

    if ( CFormMould(this , &saildefcopy.mould).exec() )
    {
        // we returned from the dialog with an 'OK'
        setSailDef(saildefcopy);
    }
}


/**
 * Displays a dialog box to open an XML sail definition.
 */
void CFormSail::slotOpen()
{
    CSailDef newdef;
    QString newname = CSailDefXmlReader().readDialog(newdef,filename);
    if ( !newname.isNull() )
    {
        filename = newname;
//        fileAccess(tr("loaded '%1'").arg(filename), filename);
        setSailDef(newdef);
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
            p.printSailData(saildef);
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


/**
 * Saves the current sail definition to an XML file.
 */
bool CFormSail::save()
{
    if ( filename.isEmpty() )
    {
        return saveAs();
    }

    // try writing to file, catch exception
    try
    {
        CSailDefXmlWriter().write(saildef, filename);
        return true;
    }
    catch (CException e)
    {
        QMessageBox::information( this, tr("error"), tr("There was an error writing to the selected file") );
    }
    return false;
}


/**
 * Opens a dialog to select the XML to which the sail definition should be saved.
 */
bool CFormSail::saveAs()
{
    QString newname = CSailDefXmlWriter().writeDialog(saildef, filename);

    if ( !newname.isNull() )
    {
        filename = newname;
        return true;
    }
    return false;
}

