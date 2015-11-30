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

#ifndef FORMSAIL_H
#define FORMSAIL_H

#include "formmain.h"
#include "sailwriter-xml.h"


/** Dialog holding a sail.
 *
 *  @see CFormSailDef for data input and checking, CSailWorker, CSailDef
 */
class CFormSail : public CFormMain
{
    Q_OBJECT

public:
    CFormSail(QWidget *parent = 0);

protected:
    QString getSaveFileName(const QString &filename)
    { return writer.getSaveFileName(filename); };
    bool read(const QString &filename);
    bool write(const QString &filename);

private slots:
    void slotDef();
    void slotExportDXF();
    void slotExportDXFSplit();
    void slotExportSVG();
    void slotExportTXT();
    void slotExportXML();
    void slotExportFlatCarlson();
    void slotExportFlatDXF();
    void slotExportFlatDXFSplit();
    void slotExportFlatDXFBlocks();
    void slotExportFlatHand();
    void slotExportFlatSVG();
    void slotExportFlatTXT();
    void slotExportFlatXML();
    void slotMould();

    void slotPrintData();
    void slotPrintDwg();
    void slotPrintDev();

    void languageChange();

private:
    void setDef(const CSailDef& newdef);
    void setupMenuBar();
    void setupMainWidget();

    CSailDef def;
    CSailDefXmlWriter writer;

    /** The sail */
    CPanelGroup sail;
    /** The developed sail */
    CPanelGroup flatsail;
    /** The display version of the flat sail */
    CPanelGroup dispsail;

    // menus and action
    /** The Print submenu */
    QMenu *menuPrint;
    /** Export 3d submenu */
    QMenu *menuExport3d;
    /** Export flat panels submenu */
    QMenu *menuExportFlat;

    /** Export 3D sail to DXF */
    QAction *actionExport3dDXF;
    /** Export 3D sail to DXF (one file per file) */
    QAction *actionExport3dDXFSplit;
    /** Export 3D sail to SVG (2D reduction) */
    QAction *actionExport3dSVG;
    /** Export 3D sail to text */
    QAction *actionExport3dTXT;
    /** Export 3D sail to XML */
    QAction *actionExport3dXML;
    /** Export flat sail to Carlson plotter format */
    QAction *actionExportFlatCarlson;
    /** Export flat sail to hand format */
    QAction *actionExportFlatHand;
    /** Export flat sail to DXF */
    QAction *actionExportFlatDXF;
    /** Export flat sail to DXF (one file per file) */
    QAction *actionExportFlatDXFSplit;
    /** Export flat sail to DXF blocks */
    QAction *actionExportFlatDXFBlocks;
    /** Export flat sail to SVG */
    QAction *actionExportFlatSVG;
    /** Export flat sail to TXT */
    QAction *actionExportFlatTXT;
    /** Export flat sail to XML */
    QAction *actionExportFlatXML;
    /** Print sail data */
    QAction *actionPrintData;
    /** Print sail development */
    QAction *actionPrintDev;
    /** Print sail drawing */
    QAction *actionPrintDwg;
    /** View sail definition */
    QAction *actionViewDef;
    /** View sail mould */
    QAction *actionViewMould;
    /** View sail definition */
    QAction *actionViewPatch;
};

#endif
