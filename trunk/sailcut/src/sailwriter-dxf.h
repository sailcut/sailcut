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

#ifndef SAILWRITER_DXF_H
#define SAILWRITER_DXF_H

#include "filewriter.h"
#include "sail.h"
#include <iostream>
#include <fstream>

/** An abstract class containing the methods needed for DXF writing.
 */
class CSailDxfWriter : public CFileWriter
{
protected:
    /** the sail to write */
    CSail _sail;
    /** the output stream */
    ofstream _out;

public:
    /** The constructor.
     *
     * @param sail the sail to write
     */
    CSailDxfWriter(const CSail &sail)
            : CFileWriter(sail.sailID, ".dxf", "DXF files"), _sail(sail)
    {}
    ;

    void writeAtom(int code, const QString& content);
    void writeFace(CPoint3d p0, CPoint3d p1, CPoint3d p2, unsigned int layer);
    void writePolyline( unsigned int layer, unsigned int color);
    void writeVertex(CPoint3d p0, unsigned int layer);

};


/** A class used to write a CSail to a simple 2D DXF file.
 */
class CSailDxfWriter2d : public CSailDxfWriter
{
public:
    /** The constructor.
     *
     * @param sail the sail to write
     */
    CSailDxfWriter2d(const CSail &sail)
            : CSailDxfWriter(sail)
    {}
    ;

    void write(const QString &filename);
    void writePanel(unsigned int panel);
};


/** A class used to write a CSail to a 2D DXF file with one block per panel.
 */
class CSailDxfWriter2dBlocks : public CSailDxfWriter2d
{
public:
    /** The constructor.
     *
     * @param sail the sail to write
     */
    CSailDxfWriter2dBlocks(const CSail &sail)
            : CSailDxfWriter2d(sail)
    {}
    ;

    void write(const QString &filename);
};



/** A class used to write a CSail to a 3D DXF file.
 */
class CSailDxfWriter3d : public CSailDxfWriter
{
public:
    /** The constructor.
     *
     * @param sail the sail to write
     */
    CSailDxfWriter3d(const CSail &sail)
            : CSailDxfWriter(sail)
    {}
    ;

    void write(const QString &filename);
    void writePanel(unsigned int panel);
};


#endif
