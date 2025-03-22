/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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

#include "sailwriter-dxf.h"
#include <QFile>

#define DXF_ENTITY  0
#define DXF_NAME    2
#define DXF_LINE    6
#define DXF_LAYER   8
#define DXF_X       10
#define DXF_Y       20
#define DXF_Z       30
#define DXF_COLOR   62
#define DXF_FLAG    70
#define DXF_COMMENT 999

#define DXF_BLACK   "0"
#define DXF_RED     "1"
#define DXF_YELLOW  "2"
#define DXF_GREEN   "3"
#define DXF_CYAN    "4"
#define DXF_BLUE    "5"
#define DXF_MAGENTA "6"
#define DXF_WHITE   "7"

/***********************************

           DXF components

***********************************/

/** Write a DXF atom to the file output stream.
 *
 * @param out the output stream
 * @param code atom code
 * @param content atom content
 */
void CSailDxfWriter::writeAtom(std::ofstream &out, int code, const QString& content) const
{
    out << code << std::endl << std::string(content.toLocal8Bit()) << std::endl;
}


/** Open the given file, then write comment and header section.
 *
 * @param out the output stream
 * @param filename the output file name
 */
void CSailDxfWriter::writeBegin(std::ofstream &out, const QString &filename) const
{
    out.open(QFile::encodeName(filename), std::ios::out);
    if (!out.is_open())
        throw write_error("CSailDxfWriter::writeBegin : unable to write to specified file");

    // write comment
    writeAtom(out, DXF_COMMENT, "DXF created by Sailcut CAD");

    // write header section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "HEADER");
    writeAtom(out, DXF_ENTITY, "ENDSEC");
}


/** Write end of file then close file.
 *
 * @param out the output stream
 */
void CSailDxfWriter::writeEnd(std::ofstream &out) const
{
    writeAtom(out, DXF_ENTITY, "EOF");
    out.close();
}


/** Write a triangular face to the file output stream.
 *
 * @param out the output stream
 * @param p0 first point
 * @param p1 second point
 * @param p2 third point
 * @param layer
 */
void CSailDxfWriter::writeFace(std::ofstream &out, CPoint3d p0, CPoint3d p1, CPoint3d p2, unsigned int layer) const
{
    // skip empty face
    real area = CVector3d::crossProduct(p1 - p0, p2 - p0).length();
    //std::cout << "area : " << area << std::endl;
    if ( area <= EPS )
    {
        std::cout << "CSailDxfWriter::writeFace : skipping empty face" << std::endl;
        return;
    }

    writeAtom(out, DXF_ENTITY, "3DFACE");
    // set the layer
    writeAtom(out, DXF_LAYER, QString::number(layer));
    // set the points
    writeAtom(out, DXF_X, QString::number(p0.x()));
    writeAtom(out, DXF_Y, QString::number(p0.y()));
    writeAtom(out, DXF_Z, QString::number(p0.z()));

    writeAtom(out, DXF_X + 1, QString::number(p1.x()));
    writeAtom(out, DXF_Y + 1, QString::number(p1.y()));
    writeAtom(out, DXF_Z + 1, QString::number(p1.z()));

    writeAtom(out, DXF_X + 2, QString::number(p2.x()));
    writeAtom(out, DXF_Y + 2, QString::number(p2.y()));
    writeAtom(out, DXF_Z + 2, QString::number(p2.z()));

    // duplicate last point for stupid AutoCAD
    writeAtom(out, DXF_X + 3, QString::number(p2.x()));
    writeAtom(out, DXF_Y + 3, QString::number(p2.y()));
    writeAtom(out, DXF_Z + 3, QString::number(p2.z()));
}


/** Write a DXF layer to the file output stream.
 *
 * @param out the output stream
 * @param layer
 * @param color the color
 */
void CSailDxfWriter::writeLayer(std::ofstream &out, unsigned int layer, const QString &color) const
{
    writeAtom(out, DXF_ENTITY, "LAYER");
    writeAtom(out, DXF_NAME, QString::number(layer));
    writeAtom(out, DXF_FLAG, "64");
    writeAtom(out, DXF_COLOR, color);
    writeAtom(out, DXF_LINE, "CONTINUOUS");
}


/** Write a DXF Polyline header to the file output stream.
 *
 * @param out the output stream
 * @param layer
 * @param color the color
 */
void CSailDxfWriter::writePolyline(std::ofstream &out, unsigned int layer, const QString &color) const
{
    writeAtom(out, DXF_ENTITY, "POLYLINE");
    // set the layer
    writeAtom(out, DXF_LAYER, QString::number(layer));
    // set color
    writeAtom(out, DXF_COLOR, color);
    // set vertice follows flag
    writeAtom(out, 66, "1");
    // set line type
    writeAtom(out, DXF_LINE, "CONTINUOUS");
}


/** Write a 2D DXF Vertex to the file output stream.
 *
 * @param out the output stream
 * @param pt point
 * @param layer
 */
void CSailDxfWriter::writeVertex(std::ofstream &out, CPoint3d pt, unsigned int layer) const
{
    writeAtom(out, DXF_ENTITY, "VERTEX");
    // set the layer
    writeAtom(out, DXF_LAYER, QString::number(layer));
    // set 3D flag
    //writeAtom(out, DXF_FLAG, "32"); // flag 3D vertex

    // set the points
    writeAtom(out, DXF_X, QString::number(pt.x()));
    writeAtom(out, DXF_Y, QString::number(pt.y()));
    //writeAtom(out, DXF_Z, QString::number(pt.z())); // for 3D
}



/***********************************

           2D DXF export

***********************************/

/** Writes a CPanelGroup to a simple DXF file.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter2d::write(const CPanelGroup &sail, const QString &filename) const
{
    switch (type)
    {
    case NORMAL:
        writeNormal(sail, filename);
        break;
    case BLOCKS:
        writeBlocks(sail, filename);
        break;
    case SPLIT:
        writeSplit(sail, filename);
        break;
    }
}

/** Writes a CPanelGroup to a simple 2D DXF file, one panel per layer.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter2d::writeNormal(const CPanelGroup &sail, const QString &filename) const
{
    std::ofstream out;
    unsigned int pn;

    // open file, write comment and header
    writeBegin(out, filename);

    // write tables section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "TABLES");
    writeAtom(out, DXF_ENTITY, "TABLE");
    writeAtom(out, DXF_NAME, "LAYER");
    writeAtom(out, DXF_FLAG, "6");

    for (pn = 0; pn < sail.size(); pn++)
        writeLayer(out, pn+1, DXF_GREEN);

    writeAtom(out, DXF_ENTITY, "ENDTAB");
    writeAtom(out, DXF_ENTITY, "ENDSEC");

    // write entities section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "ENTITIES");

    // loop over panels ////////
    for (pn = 0; pn < sail.size(); pn++)
        writePanel(out, sail[pn], pn+1);

    writeAtom(out, DXF_ENTITY, "ENDSEC");

    // end of file
    writeEnd(out);
}


/** Writes a CPanelGroup to a 2D DXF file with one block per panel on different layer.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter2d::writeBlocks(const CPanelGroup &sail, const QString &filename) const
{
    std::ofstream out;
    unsigned int pn;

    // open file, write comment and header
    writeBegin(out, filename);

    // write tables section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "TABLES");
    writeAtom(out, DXF_ENTITY, "TABLE");
    writeAtom(out, DXF_NAME, "LAYER");
    writeAtom(out, DXF_FLAG, "16");
    for (pn = 0; pn < sail.size(); pn++)
        writeLayer(out, pn+1, DXF_GREEN);
    writeAtom(out, DXF_ENTITY, "ENDTAB");
    writeAtom(out, DXF_ENTITY, "ENDSEC");

    // write entities section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "BLOCKS");
    for (pn = 0; pn < sail.size(); pn++)
    {
        writeAtom(out, DXF_ENTITY, "BLOCK");
        writeAtom(out, 100, "AcDbEntity");
        writeAtom(out, DXF_LAYER, QString::number(pn+1));
        writeAtom(out, 100, "AcDbBlockBegin");
        writeAtom(out, DXF_NAME, "panel "+QString::number(pn+1));
        writeAtom(out, DXF_FLAG, "1");
        writeAtom(out, DXF_X, "0");
        writeAtom(out, DXF_Y, "0");
        writeAtom(out, DXF_Z, "0");
        writeAtom(out, 3, "panel "+QString::number(pn+1));

        writePanel(out, sail[pn], pn+1);

        writeAtom(out, DXF_ENTITY, "ENDBLCK");
        writeAtom(out, 100, "AcDbBlockEnd");
    }
    writeAtom(out, DXF_ENTITY, "ENDSEC"); // end of BLOCKS section

    // end of file
    writeEnd(out);
}


/** Writes a CPanelGroup to a 2D DXF file with one file per panel.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter2d::writeSplit(const CPanelGroup &sail, const QString &basename) const
{
    std::ofstream out;
    unsigned int pn;

    for (pn = 0; pn < sail.size(); pn++)
    {
        QString filename = basename;
        filename.replace(".dxf", QString("-%1.dxf").arg(pn));

        // open file, write comment and header
        writeBegin(out, filename);

        // write tables section
        writeAtom(out, DXF_ENTITY, "SECTION");
        writeAtom(out, DXF_NAME, "TABLES");
        writeAtom(out, DXF_ENTITY, "TABLE");
        writeAtom(out, DXF_NAME, "LAYER");
        writeAtom(out, DXF_FLAG, "6");
        writeLayer(out, 1, DXF_CYAN);
        writeAtom(out, DXF_ENTITY, "ENDTAB");
        writeAtom(out, DXF_ENTITY, "ENDSEC");

        // write entities section
        writeAtom(out, DXF_ENTITY, "SECTION");
        writeAtom(out, DXF_NAME, "ENTITIES");
        writePanel(out, sail[pn], 1);
        writeAtom(out, DXF_ENTITY, "ENDSEC");

        // end of file
        writeEnd(out);
    }
}


/** Write a single 2D DXF developed panel draw and cut edges to the file output stream.
 *
 * @param out the output stream
 * @param panel the panel to be written
 * @param layer the layer on which the panel is written
 */
void CSailDxfWriter2d::writePanel(std::ofstream &out, const CPanel &panel, unsigned int layer) const
{
    //writeAtom(out, DXF_COMMENT, panel.label.name);

    CSide top = panel.top;
    CSide btm = panel.bottom;
    CSide left = panel.left;
    CSide right = panel.right;

    CSide ctop = panel.cutTop;
    CSide cbtm = panel.cutBottom;
    CSide cleft = panel.cutLeft;
    CSide cright = panel.cutRight;

    CPoint3d pt;
    CVector3d V;
    unsigned int i=0;
    int j=0;

    //// polyline header for draw line
    writePolyline(out, layer, DXF_BLUE);

    // left edge
    pt = left[0];
    writeVertex(out, pt, layer);

    for (i = 1; i < left.size(); i++)
    {
        V= left[i] - pt;
        if (V.length() > EPS)
        {
            pt = left[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel top edge
    for (i = 0; i < top.size(); i++)
    {
        V = top[i] - pt;
        if (V.length() > EPS)
        {
            pt = top[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel right edge
    for (j = right.size()-1; j > - 1; j--)
    {
        V = right[j] - pt;
        if (V.length() > EPS)
        {
            pt = right[j];
            writeVertex(out, pt, layer);
        }
    }

    // panel bottom edge
    for (j = btm.size()-1; j > -1; j--)
    {
        V = btm[j] - pt;
        if (V.length() > EPS)
        {
            pt = btm[j];
            writeVertex(out, pt, layer);
        }
    }
    // close the circuit to start point
    pt = left[0];
    writeVertex(out, pt, layer);

    writeAtom(out, DXF_ENTITY, "SEQEND"); // end draw line

    //// polyline header for cut line
    writePolyline(out, layer, DXF_RED);

    // left edge
    pt = cleft[0];
    writeVertex(out, pt, layer);

    for (i = 1; i < left.size(); i++)
    {
        V= cleft[i] - pt;
        if (V.length() > EPS)
        {
            pt = cleft[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel top edge
    for (i = 0; i < top.size(); i++)
    {
        V = ctop[i] - pt;
        if (V.length() > EPS)
        {
            pt = ctop[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel right edge
    for (j = right.size()-1; j > - 1; j--)
    {
        V = cright[j] - pt;
        if (V.length() > EPS)
        {
            pt = cright[j];
            writeVertex(out, pt, layer);
        }
    }

    // panel bottom edge
    for (j = btm.size()-1; j > -1; j--)
    {
        V = cbtm[j] - pt;
        if (V.length() > EPS)
        {
            pt = cbtm[j];
            writeVertex(out, pt, layer);
        }
    }
    // close the circuit to start point
    pt = cleft[0];
    writeVertex(out, pt, layer);

    writeAtom(out, DXF_ENTITY, "SEQEND"); // end cut line
}



/***********************************

           3D DXF export

***********************************/

/** Writes a 3D CPanelGroup to a 3D DXF file.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter3d::write(const CPanelGroup &sail, const QString &filename) const
{
    switch (type)
    {
    case NORMAL:
        writeNormal(sail, filename);
        break;
    case SPLIT:
        writeSplit(sail, filename);
        break;
    }
}

/** Writes a 3D CPanelGroup to a 3D DXF file with one panel per layer.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter3d::writeNormal(const CPanelGroup &sail, const QString &filename) const
{
    std::ofstream out;
    unsigned int pn;

    // open file, write comment and header
    writeBegin(out, filename);

    // write tables section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "TABLES");
    writeAtom(out, DXF_ENTITY, "TABLE");
    writeAtom(out, DXF_NAME, "LAYER");
    writeAtom(out, DXF_FLAG, "6");
    for (pn = 0; pn < sail.size(); pn++)
        writeLayer(out, pn + 1, (pn % 2) ? DXF_YELLOW : DXF_CYAN);
    writeAtom(out, DXF_ENTITY, "ENDTAB");
    writeAtom(out, DXF_ENTITY, "ENDSEC");

    // write entities section
    writeAtom(out, DXF_ENTITY, "SECTION");
    writeAtom(out, DXF_NAME, "ENTITIES");
    for (pn = 0; pn < sail.size(); pn++)
        writePanel(out, sail[pn], pn + 1);
    writeAtom(out, DXF_ENTITY, "ENDSEC");

    // end of file
    writeEnd(out);
}


/** Write a single 3D DXF panel to the file output stream.
 *
 * @param out the output stream
 * @param panel the number of the panel to be written
 * @param layer the layer on which the panel is written
 */
void CSailDxfWriter3d::writePanel(std::ofstream &out, const CPanel &panel, unsigned int layer) const
{
    //writeAtom(out, DXF_COMMENT, panel.label.name);
    //writeAtom(out, DXF_COMMENT, QString("panel : ") + QString::number(panel));
    CSide top = panel.top;
    CSide btm = panel.bottom;
    CSide left = panel.left;
    CSide right = panel.right;
    CPoint3d pt;
    unsigned int i=0;

    // left triangle fan
    pt = (left[0]+left[left.size()-1])*0.5;
    for (i = 1; i < left.size(); i++)
        writeFace(out, pt, left[i-1], left[i], layer);

    // panel triangle strip
    for (i = 1; i < top.size(); i++)
    {
        writeFace(out, top[i-1], btm[i-1], top[i], layer);
        writeFace(out, top[i], btm[i], btm[i-1], layer);
    }

    // right triangle fan
    pt = (right[0]+right[right.size()-1])*0.5;
    for (i = 1; i < right.size(); i++)
        writeFace(out, pt, right[i-1], right[i], layer);
}


/** Writes a 3D CPanelGroup to a 3D DXF file with one file per panel.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter3d::writeSplit(const CPanelGroup &sail, const QString &basename) const
{
    std::ofstream out;

    for (unsigned int pn = 0; pn < sail.size(); pn++)
    {
        QString filename = basename;
        filename.replace(".dxf", QString("-%1.dxf").arg(pn));

        // open file, write comment and header
        writeBegin(out, filename);

        // write tables section
        writeAtom(out, DXF_ENTITY, "SECTION");
        writeAtom(out, DXF_NAME, "TABLES");
        writeAtom(out, DXF_ENTITY, "TABLE");
        writeAtom(out, DXF_NAME, "LAYER");
        writeAtom(out, DXF_FLAG, "6");
        writeLayer(out, 1, DXF_CYAN);
        writeAtom(out, DXF_ENTITY, "ENDTAB");
        writeAtom(out, DXF_ENTITY, "ENDSEC");

        // write entities section
        writeAtom(out, DXF_ENTITY, "SECTION");
        writeAtom(out, DXF_NAME, "ENTITIES");
        writePanel(out, sail[pn], 1);
        writeAtom(out, DXF_ENTITY, "ENDSEC");

        // end of file
        writeEnd(out);
    }
}
