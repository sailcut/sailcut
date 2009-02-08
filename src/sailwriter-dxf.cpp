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

#include "sailwriter-dxf.h"
#include <QFile>

/***********************************

           DXF components

***********************************/

/** Write a DXF atom to the file output stream.
 *
 * @param out the output stream
 * @param code atom code
 * @param content atom content
 */
void CSailDxfWriter::writeAtom(ofstream &out, int code, const QString& content) const
{
    out << code << endl << string(content.toLocal8Bit()) << endl;
}


/** Open the given file, then write comment and header section.
 *
 * @param out the output stream
 * @param filename the output file name
 */
void CSailDxfWriter::writeBegin(ofstream &out, const QString &filename) const
{
    out.open(QFile::encodeName(filename),ios::out);
    if (!out.is_open())
        throw write_error("CSailDxfWriter::writeBegin : unable to write to specified file");

    // write comment
    writeAtom(out, 999, "DXF created by Sailcut CAD");

    // write header section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "HEADER");
    writeAtom(out, 0, "ENDSEC");
}


/** Write end of file then close file.
 *
 * @param out the output stream
 */
void CSailDxfWriter::writeEnd(ofstream &out) const
{
    writeAtom(out, 0, "EOF");
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
void CSailDxfWriter::writeFace(ofstream &out, CPoint3d p0, CPoint3d p1, CPoint3d p2, unsigned int layer) const
{
    // skip empty face
    real area = CVector3d(p1 -p0).cross(p2 - p0).norm();
    //cout << "area : " << area << endl;
    if ( area < EPS )
    {
        cout << "CSailDxfWriter::writeFace : skipping empty face" << endl;
        return;
    }

    writeAtom(out, 0, "3DFACE");
    // set the layer
    writeAtom(out, 8, QString::number(layer));
    // set the points
    writeAtom(out, 10, QString::number(p0.x()));
    writeAtom(out, 20, QString::number(p0.y()));
    writeAtom(out, 30, QString::number(p0.z()));

    writeAtom(out, 11, QString::number(p1.x()));
    writeAtom(out, 21, QString::number(p1.y()));
    writeAtom(out, 31, QString::number(p1.z()));

    writeAtom(out, 12, QString::number(p2.x()));
    writeAtom(out, 22, QString::number(p2.y()));
    writeAtom(out, 32, QString::number(p2.z()));

    // duplicate last point for stupid AutoCAD
    writeAtom(out, 13, QString::number(p2.x()));
    writeAtom(out, 23, QString::number(p2.y()));
    writeAtom(out, 33, QString::number(p2.z()));
}

/** Write a DXF Polyline header to the file output stream.
 *
 * @param out the output stream
 * @param layer
 * @param color the color (red=1 blue=5 white=7)
 */
void CSailDxfWriter::writePolyline(ofstream &out, unsigned int layer, unsigned int color) const
{
    writeAtom(out, 0, "POLYLINE");
    // set the layer
    writeAtom(out, 8, QString::number(layer));
    // set color
    writeAtom(out, 62, QString::number(color));
    // set vertice follows flag
    writeAtom(out, 66, "1");
    // line type
    writeAtom(out, 6, "CONTINUOUS");
}


/** Write a 2D DXF Vertex to the file output stream.
 *
 * @param out the output stream
 * @param pt point
 * @param layer
 */
void CSailDxfWriter::writeVertex(ofstream &out, CPoint3d pt, unsigned int layer) const
{
    writeAtom(out, 0, "VERTEX");
    // set the layer
    writeAtom(out, 8, QString::number(layer));
    // set 3D flag
    //writeAtom(out, 70, "32"); // flag 3D vertex

    // set the points
    writeAtom(out, 10, QString::number(pt.x()));
    writeAtom(out, 20, QString::number(pt.y()));
    //writeAtom(out, 30, QString::number(pt.z())); // for 3D
}



/***********************************

           2D DXF export

***********************************/


/** Writes a CPanelGroup to a simple 2D DXF file.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter2d::write(const CPanelGroup &sail, const QString &filename) const
{
    ofstream out;

    // open file, write comment and header
    writeBegin(out, filename);

    // write tables section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "TABLES");
    writeAtom(out, 0, "TABLE");
    writeAtom(out, 2, "LAYER");
    writeAtom(out, 70, "16");

    unsigned int pn = 0;
    for (pn = 0; pn < sail.size(); pn++)
    {
        //writeAtom(999,_sail[pn].label.name);
        writeAtom(out, 0, "LAYER");
        writeAtom(out, 2, QString::number(pn+1));
        // flags
        writeAtom(out, 70, "64");
        // colour by default  red=1 yellow=2 blue=5 white=7
        writeAtom(out, 62, "3");

        writeAtom(out, 6, "CONTINUOUS");
    }

    writeAtom(out, 0, "ENDTAB");
    writeAtom(out, 0, "ENDSEC");

    // write entities section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "ENTITIES");

    // loop over panels ////////
    for (pn = 0; pn < sail.size(); pn++)
        writePanel(out, sail[pn], pn+1);

    writeAtom(out, 0, "ENDSEC");

    // end of file
    writeEnd(out);
}


/** Writes a CPanelGroup to a 2D DXF file with one block per panel.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailDxfWriter2dBlocks::write(const CPanelGroup &sail, const QString &filename) const
{
    ofstream out;

    // open file, write comment and header
    writeBegin(out, filename);

    // write tables section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "TABLES");
    writeAtom(out, 0, "TABLE");
    writeAtom(out, 2, "LAYER");
    writeAtom(out, 70, "16");

    unsigned int pn = 0;
    for (pn = 0; pn < sail.size(); pn++)
    {
        //writeAtom(999,_sail[pn].label.name);
        writeAtom(out, 0, "LAYER");
        writeAtom(out, 2, QString::number(pn+1));
        // flags
        writeAtom(out, 70, "64");
        // colour by default  red=1 yellow=2 blue=5 white=7
        writeAtom(out, 62, "3");

        writeAtom(out, 6, "CONTINUOUS");
    }

    writeAtom(out, 0, "ENDTAB");
    writeAtom(out, 0, "ENDSEC");

    // write entities section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "BLOCKS");
    //writeAtom(out, 2, "ENTITIES");

    // loop over panels ////////
    for (pn = 0; pn < sail.size(); pn++)
    {
        writeAtom(out, 0, "BLOCK");
        writeAtom(out, 100, "AcDbEntity");
        writeAtom(out, 8, QString::number(pn+1));
        writeAtom(out, 100, "AcDbBlockBegin");
        writeAtom(out, 2, "panel "+QString::number(pn+1));
        writeAtom(out, 70, "1");
        writeAtom(out, 10, "0");
        writeAtom(out, 20, "0");
        writeAtom(out, 30, "0");
        writeAtom(out, 3, "panel "+QString::number(pn+1));

        writePanel(out, sail[pn], pn+1);

        writeAtom(out, 0, "ENDBLCK");
        writeAtom(out, 100, "AcDbBlockEnd");
    }
    writeAtom(out, 0, "ENDSEC"); // end of BLOCKS section

    // end of file
    writeEnd(out);
}


/** Write a single 2D DXF developed panel draw and cut edges to the file output stream.
 *
 * @param out the output stream
 * @param panel the panel to write
 * @param layer
 */
void CSailDxfWriter2d::writePanel(ofstream &out, const CPanel &panel, unsigned int layer) const
{
    //writeAtom(out, 999, panel.label.name);

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
    writePolyline(out, layer, 5); // color 5=blue

    // left edge
    pt = left[0];
    writeVertex(out, pt, layer);

    for (i = 1; i < left.size(); i++)
    {
        V= left[i] - pt;
        if (V.norm()> EPS)
        {
            pt = left[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel top edge
    for (i = 0; i < top.size(); i++)
    {
        V = top[i] - pt;
        if (V.norm() > EPS)
        {
            pt = top[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel right edge
    for (j = right.size()-1; j > - 1; j--)
    {
        V = right[j] - pt;
        if (V.norm() > EPS)
        {
            pt = right[j];
            writeVertex(out, pt, layer);
        }
    }

    // panel bottom edge
    for (j = btm.size()-1; j > -1; j--)
    {
        V = btm[j] - pt;
        if (V.norm() > EPS)
        {
            pt = btm[j];
            writeVertex(out, pt, layer);
        }
    }
    // close the circuit to start point
    pt = left[0];
    writeVertex(out, pt, layer);

    writeAtom(out, 0, "SEQEND"); // end draw line

    //// polyline header for cut line
    writePolyline(out, layer, 1); // color 1=red

    // left edge
    pt = cleft[0];
    writeVertex(out, pt, layer);

    for (i = 1; i < left.size(); i++)
    {
        V= cleft[i] - pt;
        if (V.norm()> EPS)
        {
            pt = cleft[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel top edge
    for (i = 0; i < top.size(); i++)
    {
        V = ctop[i] - pt;
        if (V.norm() > EPS)
        {
            pt = ctop[i];
            writeVertex(out, pt, layer);
        }
    }

    // panel right edge
    for (j = right.size()-1; j > - 1; j--)
    {
        V = cright[j] - pt;
        if (V.norm() > EPS)
        {
            pt = cright[j];
            writeVertex(out, pt, layer);
        }
    }

    // panel bottom edge
    for (j = btm.size()-1; j > -1; j--)
    {
        V = cbtm[j] - pt;
        if (V.norm() > EPS)
        {
            pt = cbtm[j];
            writeVertex(out, pt, layer);
        }
    }
    // close the circuit to start point
    pt = cleft[0];
    writeVertex(out, pt, layer);

    writeAtom(out, 0, "SEQEND"); // end cut line
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
    ofstream out;

    // open file, write comment and header
    writeBegin(out, filename);

    // write tables section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "TABLES");
    writeAtom(out, 0, "TABLE");
    writeAtom(out, 2, "LAYER");
    writeAtom(out, 70, "6");
    for (unsigned int pn = 0; pn < sail.size(); pn++)
    {
        //writeAtom(out, 999, panel.label.name);
        writeAtom(out, 0, "LAYER");
        writeAtom(out, 2, QString::number(pn+1));
        // flags
        writeAtom(out, 70, "64");
        // colour
        if (pn%2)
            writeAtom(out, 62, "2"); // 1=red 2=yellow
        else
            writeAtom(out, 62, "4"); // 4=cyan 5=blue

        writeAtom(out, 6, "CONTINUOUS");
    }

    writeAtom(out, 0, "ENDTAB");
    writeAtom(out, 0, "ENDSEC");

    // write entities section
    writeAtom(out, 0, "SECTION");
    writeAtom(out, 2, "ENTITIES");

    // loop over panels ////////
    for (unsigned int i = 0; i < sail.size(); i++)
        writePanel(out, sail[i], i+1);

    writeAtom(out, 0, "ENDSEC");

    // end of file
    writeEnd(out);
}


/** Write a single 3D DXF panel to the file output stream.
 *
 * @param out the output stream
 * @param panel the number of the panel to write
 * @param layer
 */
void CSailDxfWriter3d::writePanel(ofstream &out, const CPanel &panel, unsigned int layer) const
{
    //writeAtom(out, 999, panel.label.name);
    //writeAtom(out, 999, QString("panel : ") + QString::number(panel));
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


