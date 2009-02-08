/*
 * Copyright (C) 1993-2008 Robert & Jeremy Laine
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

#ifndef SAILWRITER_DXF_H
#define SAILWRITER_DXF_H

#include "filewriter.h"
#include "panelgroup.h"
#include <iostream>
#include <fstream>

/** An abstract class containing the methods needed for DXF writing.
 */
class CSailDxfWriter : public CFileWriter<CPanelGroup>
{
public:
    /** The constructor.
     */
    CSailDxfWriter() : CFileWriter<CPanelGroup>(".dxf", "DXF files")
    {}
    ;

    void writeBegin(ofstream &out, const QString &filename) const;
    void writeEnd(ofstream &out) const;

    void writeAtom(ofstream &out, int code, const QString& content) const;
    void writeFace(ofstream &out, CPoint3d p0, CPoint3d p1, CPoint3d p2, unsigned int layer) const;
    void writePolyline(ofstream &out, unsigned int layer, unsigned int color) const;
    void writeVertex(ofstream &out, CPoint3d p0, unsigned int layer) const;
};


/** A class used to write a CPanelGroup to a simple 2D DXF file.
 *
 * @ingroup FileIo
 */
class CSailDxfWriter2d : public CSailDxfWriter
{
public:
    void write(const CPanelGroup &sail, const QString &filename) const;
    void writePanel(ofstream &out, const CPanel &panel, unsigned int layer) const;
};


/** A class used to write a CPanelGroup to a 2D DXF file with one block per panel.
 *
 * @ingroup FileIo
 */
class CSailDxfWriter2dBlocks : public CSailDxfWriter2d
{
public:
    void write(const CPanelGroup &sail, const QString &filename) const;
};


/** A class used to write a CPanelGroup to a 3D DXF file.
 *
 * @ingroup FileIo
 */
class CSailDxfWriter3d : public CSailDxfWriter
{
public:
    void write(const CPanelGroup &sail, const QString &filename) const;
    void writePanel(ofstream &out, const CPanel &panel, unsigned int layer) const;
};


#endif
