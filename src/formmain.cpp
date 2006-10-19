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

#include "formmain.h"
#include "formsaildef.h"
#include "formhelp.h"
#include "formmould.h"
#include "formrig.h"

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

#include "../icons/sailcut.xpm"

#include <QDir>
#include <QLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#ifdef HAVE_QDESKTOPSERVICES
#include <QDesktopServices>
#endif


/**
 * Constructs Sailcut CAD's main window.
 *
 * @param myApp the Sailcut application
 * @param parent the parent widget
 */
CFormMain::CFormMain(CSailApp *myApp, QWidget *parent)
        : QMainWindow(parent), app(myApp), prefs(&myApp->prefs)
{
    setMinimumSize( QSize( 300, 220 ) );

    // locate Handbook
    app->loadTranslation(prefs->language);
    handbook = app->findHandbook(prefs->language);
    qDebug("handbook : %s", (const char*)handbook.toString().toLocal8Bit());

    // create status bar
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    // create menu bar
    setupMenuBar();

    // create main widget
    setupMainWidget();

    // set language
    languageChange();

    // set icon
    setWindowIcon( QPixmap( (const char **)sailcut_xpm ) );

    // resize to prefered size
    resize( QSize(prefs->sailWindowWidth,prefs->sailWindowHeight).expandedTo(minimumSizeHint()) );
}


/**
 * This event is received when the user closes the dialog.
 */
void CFormMain::closeEvent(QCloseEvent *e)
{
    prefs->sailWindowHeight = height();
    prefs->sailWindowWidth = width();
    QMainWindow::closeEvent(e);
}


/**
 * We received a keypress, we pass it down to the visible tab.
 */
void CFormMain::keyPressEvent ( QKeyEvent * e )
{
    panel[tabs->currentIndex()]->keyPressEvent(e);
}


/**
 * This method is called whenever we read or write a sail definition.
 * It updates the Most Recently Used files list correspondingly.
 *
 * @param event The action performed on the file (read, write)
 * @param file The file that was accessed
 */
void CFormMain::fileAccess(QString event, QString file)
{
    statusbar->showMessage(event);
    prefs->mruSaildef.touchEntry(file);
    makeMenuMru();
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CFormMain::languageChange()
{
    setWindowTitle( "Sailcut CAD" );

    menuFile->setTitle( tr("&File") );

    actionNew->setText( tr("&New") );
    actionOpen->setText( tr("&Open") );
    menuRecent->setTitle( tr("Open &recent") );
    actionSave->setText( tr("&Save") );
    actionSaveAs->setText( tr("Save &As") );

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

    actionQuit->setText( tr("&Quit") );


    // View menu

    menuView->setTitle( tr("&View") );
    actionViewDef->setText( tr("&Dimensions") );
    actionViewMould->setText( tr("&Mould") );
    actionViewPatch->setText( tr("&Patches") );
    actionViewRig->setText( tr("&Rig") );

    // language submenu
    menuLanguage->setTitle( tr("Language") );


    // Help menu
    menuHelp->setTitle( tr("&Help") );
    actionHandbook->setText( tr("Sailcut CAD &Handbook") );
    actionAboutQt->setText( tr("About &Qt") );
    actionAbout->setText( tr("About &Sailcut CAD") );

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
 * Creates the "Open Recent" menu from the Most Recently Used files list.
 */
void CFormMain::makeMenuMru()
{
    menuRecent->clear();

    for ( unsigned int i = 0; i < prefs->mruSaildef.size(); i++)
    {
        menuRecent->addAction( prefs->mruSaildef[i], this, SLOT( slotOpenRecent() ) )->setData(i);
    }
}


/**
 * Replaces the current sail definition.
 *
 * @param newdef
 */
void CFormMain::setSailDef(const CSailDef newdef)
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
void CFormMain::setupMenuBar()
{
    // File menu
    menuFile = menuBar()->addMenu("");
    actionNew = menuFile->addAction("", this, SLOT( slotNew() ) );
    actionOpen = menuFile->addAction("", this, SLOT( slotOpen() ) );

    menuRecent = menuFile->addMenu("");

    menuFile->addSeparator();

    // print submenu
    menuPrint = menuFile->addMenu("");
    actionPrintData = menuPrint->addAction("", this, SLOT( slotPrintData() ));
    actionPrintDwg = menuPrint->addAction("", this, SLOT( slotPrintDwg() ));
    actionPrintDev = menuPrint->addAction("", this, SLOT( slotPrintDev() ));

    menuFile->addSeparator();


    actionSave = menuFile->addAction("", this, SLOT( slotSave() ) );
    actionSaveAs = menuFile->addAction("", this, SLOT( slotSaveAs() ) );

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

    menuFile->addSeparator();

    actionQuit = menuFile->addAction( "", this, SLOT( close() ) );

    // View menu

    menuView = menuBar()->addMenu("");
    actionViewDef = menuView->addAction( "", this, SLOT( slotDef() ) );
    actionViewMould = menuView->addAction( "", this, SLOT ( slotMould() ) );
    // TODO : enable the following action when the patch viewer is ready
    actionViewPatch = menuView->addAction( "" );
    actionViewPatch->setEnabled(false);
    actionViewRig = menuView->addAction( "", this, SLOT ( slotRig() ) );

    menuView->addSeparator();

    // language submenu
    menuLanguage = menuView->addMenu("");

    // language text is not to be translated
    menuLanguage->addAction( "English", this, SLOT( slotLanguage() ) )->setData("en");
    menuLanguage->addAction( QString::fromUtf8("Français"), this, SLOT( slotLanguage() ) )->setData("fr");
    menuLanguage->addAction( "Nederlands", this, SLOT( slotLanguage() ) )->setData("nl");
    menuLanguage->addAction( "Deutsch", this, SLOT( slotLanguage() ) )->setData("de");
    menuLanguage->addAction( "Italiano", this, SLOT( slotLanguage() ) )->setData("it");
    menuLanguage->addAction( "Norsk", this, SLOT( slotLanguage() ) )->setData("no");
    menuLanguage->addAction( "Dansk", this, SLOT( slotLanguage() ) )->setData("dk");
    // menuLanguage->addAction( "Svenska", this, SLOT( slotLanguage() ) )->setData("sv");
    menuLanguage->addAction( QString::fromUtf8("Portugês"), this, SLOT( slotLanguage() ) )->setData("pt");
    // menuLanguage->addAction( "Español", this, SLOT( slotLanguage() ) )->setData("es");
    menuLanguage->addAction( "Russian", this, SLOT( slotLanguage() ) )->setData("ru");
    //menuLanguage->addAction( "Polish", this, SLOT( slotLanguage() ) )->setData("pl");

    // Help menu
    menuHelp = menuBar()->addMenu("");

    actionHandbook = menuHelp->addAction( "", this, SLOT( slotHandbook() ) );
    actionAboutQt = menuHelp->addAction( "", this, SLOT( slotAboutQt() ) );
    actionAbout = menuHelp->addAction( "", this, SLOT( slotAbout() ) );

}


/**
 * Creates the main widget
 */
void CFormMain::setupMainWidget()
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
void CFormMain::show(const QString newname)
{
    // load preferences
    makeMenuMru();

    // load or create sail
    CSailDef newdef;
    filename = newname;
    if ( !filename.isNull() )
    {
        newdef = CSailDefXmlReader("saildef").read(filename);
        fileAccess(tr("loaded '%1'").arg(filename),filename);
    }
    else
    {
        statusbar->showMessage( tr("created new sail") );
    }
    setSailDef(newdef);
    QMainWindow::show();
}


/**
 * Displays the "About Sailcut CAD" dialog box
 */
void CFormMain::slotAbout()
{
    QMessageBox::about( this, tr("About Sailcut CAD"),

                        "<h2>Sailcut CAD"
#ifdef VERSION
                        " "VERSION
#endif
                        "</h2>"
                        "<p>Sailcut is a software for designing boat sails<br/>"
                        "(C) 1993-2006 Robert & Jeremy Laine"

                        "<p>For more information, visit the project's page at <a href=\"http://sailcut.sourceforge.net/\">http://sailcut.sourceforge.net/</a>.</p>"

                        "<p>Sailcut is a trademark registered by Robert Laine</p>"

                        "<table border=\"1\"><tr><td><small>"
                        "This program is free software; you can redistribute it and/or modify "
                        "it under the terms of the GNU General Public License as published by "
                        "the Free Software Foundation; either version 2 of the License, or "
                        "(at your option) any later version.<br/><br/>"

                        "This program is distributed in the hope that it will be useful, "
                        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                        "GNU General Public License for more details.<br/><br/>"

                        "You should have received a copy of the GNU General Public License "
                        "along with this program; if not, write to the Free Software "
                        "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA"
                        "</small></td></tr></table>"
                      );
}


/**
 * Displays the "About Qt" dialog box
 */
void CFormMain::slotAboutQt()
{
    QMessageBox::aboutQt( this );
}



/**
 * Displays the sail CFormSailDef sail definition dialog.
 */
void CFormMain::slotDef()
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
void CFormMain::slotExportDXF()
{
    CSailDxfWriter3d(sail).writeDialog();
}


/**
 * Exports the 3D sail to a TXT file.
 */
void CFormMain::slotExportTXT()
{
    CSailTxtWriter(sail).writeDialog();
}


/**
 * Exports the 3D sail to an XML file.
 */
void CFormMain::slotExportXML()
{
    CPanelGroupXmlWriter(sail, "sail").writeDialog();
}


/**
 * Exports the flat sail to a Carlson plotter file
 */
void CFormMain::slotExportFlatCarlson()
{
    CSailCarlsonWriter(flatsail).writeDialog();
}


/**
 * Exports the flat sail with panels staggered as displayed to a DXF file
  */
void CFormMain::slotExportFlatDXF()
{
    CSailDxfWriter2d(dispsail).writeDialog();
}


/**
 * Exports the flat sail with panels superimposed to a DXF file with blocks
  */
void CFormMain::slotExportFlatDXFBlocks()
{
    CSailDxfWriter2d(flatsail).writeDialog();
}


/**
 * Exports the flat sail to a "hand" sail file.
 */
void CFormMain::slotExportFlatHand()
{
    CSailHandWriter(flatsail).writeDialog();
}


/**
 * Exports the flat sail to a TXT sail file.
 */
void CFormMain::slotExportFlatTXT()
{
    CSailTxtWriter(flatsail).writeDialog();
}


/**
 * Exports the flat sail to an XML sail file
 */
void CFormMain::slotExportFlatXML()
{
    CPanelGroupXmlWriter(flatsail,"sail").writeDialog();
}


/**
 * Display the Sailcut handbook.
 */
void CFormMain::slotHandbook()
{
    //qDebug("handbook : %s", (const char*)handbook.toLocal8Bit());
    if ( !handbook.isEmpty() )
    {
#ifdef HAVE_QDESKTOPSERVICES
        if (QDesktopServices::openUrl(handbook))
            return;
#endif
        CFormHelp(this , prefs , handbook).exec();
    }
}


/**
 * Switches the current language.
 */
void CFormMain::slotLanguage()
{
    QString locale;

    QAction *a = qobject_cast<QAction *>(sender());
    if ( !a )
        return;

    locale = a->data().toString();

    prefs->language = locale;
    app->loadTranslation(locale);

    // try to locate handbook
    handbook = app->findHandbook(locale);

    languageChange();
}


/**
 * Displays the CFormMould sail mould definition dialog.
 */
void CFormMain::slotMould()
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
 * Creates a new sail
 */
void CFormMain::slotNew()
{
    filename = "";
    setSailDef(CSailDef());
    statusbar->showMessage( tr("created new sail") );
}


/**
 * Displays a dialog box to open an XML sail definition.
 */
void CFormMain::slotOpen()
{
    CSailDef newdef;
    QString newname = CSailDefXmlReader("saildef").readDialog(newdef,filename);
    if ( !newname.isNull() )
    {
        filename = newname;
        fileAccess(tr("loaded '%1'").arg(filename), filename);
        setSailDef(newdef);
    }
}


/**
 * Opens a recently used sail definition.
 */
void CFormMain::slotOpenRecent()
{
    // retrieve the index of the MRU entry
    QAction *a = qobject_cast<QAction *>(sender());
    if ( !a )
        return;
    int index = a->data().toInt();

    filename = prefs->mruSaildef[index];
    try
    {
        setSailDef(CSailDefXmlReader("saildef").read(filename));
        fileAccess(tr("loaded '%1'").arg(filename), filename);
    }
    catch (CException e)
    {
        prefs->mruSaildef.removeEntry(filename);
        makeMenuMru();
        statusbar->showMessage( tr("error loading '%1'").arg(filename) );
    }
}


/**
 * Print the current sail data.
 */
void CFormMain::slotPrintData()
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
void CFormMain::slotPrintDev()
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
void CFormMain::slotPrintDwg()
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
 * Display the rig dialog
 */
void CFormMain::slotRig()
{
    CFormRig *frmRig = new CFormRig(app, this);
    frmRig->show();
}


/**
 * Saves the current sail definition to an XML file.
 */
void CFormMain::slotSave()
{
    if ( filename.isEmpty() )
    {
        slotSaveAs();
        return;
    }

    // try writing to file, catch exception
    try
    {
        CSailDefXmlWriter(saildef , "saildef").write(filename);
        fileAccess(tr("wrote '%1'").arg(filename),filename);
    }
    catch (CException e)
    {
        QMessageBox::information( this, tr("error"), tr("There was an error writing to the selected file") );
    }
}


/**
 * Opens a dialog to select the XML to which the sail definition should be saved.
 */
void CFormMain::slotSaveAs()
{
    QString newname = CSailDefXmlWriter(saildef , "saildef").writeDialog(filename);

    if ( !newname.isNull() )
    {
        filename = newname;
        fileAccess(tr("wrote '%1'").arg(filename), filename);
    }
}

