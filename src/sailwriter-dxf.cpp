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

#include "sailwriter-dxf.h"
#include <QFile>

/***********************************
 
           DXF components
 
***********************************/

/** Write a DXF atom to the file output stream.
 *
 * @param code atom code
 * @param content atom content
 */
void CSailDxfWriter::writeAtom(int code, const QString& content)
{
    _out << code << endl << string(content.toLocal8Bit()) << endl;
}


/** Write a triangular face to the file output stream.
 *
 * @param p0 first point
 * @param p1 second point
 * @param p2 third point
 * @param layer
 */
void CSailDxfWriter::writeFace(CPoint3d p0, CPoint3d p1, CPoint3d p2, unsigned int layer)
{
    // skip empty face
    real area = CVector3d(p1 -p0).cross(p2 - p0).norm();
    //cout << "area : " << area << endl;
    if ( area < EPS )
    {
        cout << "CSailDxfWriter::writeFace : skipping empty face" << endl;
        return;
    }

    writeAtom(0,"3DFACE");
    // set the layer
    writeAtom(8, QString::number(layer));
    // set the points
    writeAtom(10,QString::number(p0.x()));
    writeAtom(20,QString::number(p0.y()));
    writeAtom(30,QString::number(p0.z()));

    writeAtom(11,QString::number(p1.x()));
    writeAtom(21,QString::number(p1.y()));
    writeAtom(31,QString::number(p1.z()));

    writeAtom(12,QString::number(p2.x()));
    writeAtom(22,QString::number(p2.y()));
    writeAtom(32,QString::number(p2.z()));

    // duplicate last point for stupid AutoCAD
    writeAtom(13,QString::number(p2.x()));
    writeAtom(23,QString::number(p2.y()));
    writeAtom(33,QString::number(p2.z()));
}


/** Write a DXF Polyline header to the file output stream.
 *
 * @param layer
 * @param color
 * red=1 blue=5 white=7
 */
void CSailDxfWriter::writePolyline( unsigned int layer, unsigned int color)
{
    writeAtom(0,"POLYLINE");
    // set the layer
    writeAtom(8, QString::number(layer));
    // set color
    writeAtom(62, QString::number(color));
    // set vertice follows flag
    writeAtom(66, "1");
    // line type
    writeAtom(6, "CONTINUOUS");
}


/** Write a 2D DXF Vertex to the file output stream.
 *
 * @param pt  point
 * @param layer
 */
void CSailDxfWriter::writeVertex(CPoint3d pt, unsigned int layer)
{
    writeAtom(0,"VERTEX");
    // set the layer
    writeAtom(8, QString::number(layer));
    // set 3D flag
    //writeAtom(70, "32"); // flag 3D vertex

    // set the points
    writeAtom(10,QString::number(pt.x()));
    writeAtom(20,QString::number(pt.y()));
    //writeAtom(30,QString::number(pt.z())); // for 3D
}



/***********************************
 
           2D DXF export
 
***********************************/


/** Writes a CSail to a simple 2D DXF file.
 *
 * @param filename the file to write to
 */
void CSailDxfWriter2d::write(const QString &filename)
{
    _out.open(QFile::encodeName(filename),ios::out);
    if (!_out.is_open())
        throw CException("CSailDxfWriter::write : unable to write to specified file");

    // write comment
    writeAtom(999,"DXF 2d created by Sailcut CAD");

    // write header section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "HEADER");
    writeAtom( 0, "ENDSEC");

    // write tables section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "TABLES");
    writeAtom( 0, "TABLE");
    writeAtom( 2, "LAYER");
    writeAtom(70, "16");

    unsigned int pn = 0;
    for ( pn = 0; pn < _sail.panel.size(); pn++)
    {
        //writeAtom(999,_sail.panel[pn].label.name);
        writeAtom( 0, "LAYER");
        writeAtom( 2, QString::number(pn+1));
        // flags
        writeAtom(70, "64");
        // colour by default  red=1 yellow=2 blue=5 white=7
        writeAtom(62, "3");

        writeAtom( 6, "CONTINUOUS");
    }

    writeAtom( 0, "ENDTAB");
    writeAtom( 0, "ENDSEC");

    // write entities section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "ENTITIES");

    // loop over panels ////////
    for ( pn = 0; pn < _sail.panel.size(); pn++)
        writePanel(pn);

    writeAtom( 0, "ENDSEC");

    // end of file
    writeAtom( 0, "EOF");
    _out.close();
}


/** Writes a CSail to a 2D DXF file with one block per panel.
 *
 * @param filename the file to write to
 */
void CSailDxfWriter2dBlocks::write(const QString &filename)
{
    _out.open(QFile::encodeName(filename),ios::out);
    if (!_out.is_open())
        throw CException("CSailDxfWriter::write : unable to write to specified file");

    // write comment
    writeAtom(999,"DXF 2d created by Sailcut CAD");

    // write header section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "HEADER");
    writeAtom( 0, "ENDSEC");

    // write tables section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "TABLES");
    writeAtom( 0, "TABLE");
    writeAtom( 2, "LAYER");
    writeAtom(70, "16");

    unsigned int pn = 0;
    for ( pn = 0; pn < _sail.panel.size(); pn++)
    {
        //writeAtom(999,_sail.panel[pn].label.name);
        writeAtom( 0, "LAYER");
        writeAtom( 2, QString::number(pn+1));
        // flags
        writeAtom(70, "64");
        // colour by default  red=1 yellow=2 blue=5 white=7
        writeAtom(62, "3");

        writeAtom( 6, "CONTINUOUS");
    }

    writeAtom( 0, "ENDTAB");
    writeAtom( 0, "ENDSEC");

    // write entities section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "BLOCKS");
    //writeAtom( 2, "ENTITIES");

    // loop over panels ////////
    for ( pn = 0; pn < _sail.panel.size(); pn++)
    {
        writeAtom( 0, "BLOCK");
        writeAtom( 100, "AcDbEntity");
        writeAtom( 8, QString::number(pn+1));
        writeAtom( 100, "AcDbBlockBegin");
        writeAtom( 2, "panel "+QString::number(pn+1));
        writeAtom( 70, "1");
        writeAtom( 10, "0");
        writeAtom( 20, "0");
        writeAtom( 30, "0");
        writeAtom( 3, "panel "+QString::number(pn+1));

        writePanel(pn);

        writeAtom( 0, "ENDBLCK");
        writeAtom( 100, "AcDbBlockEnd");
    }
    writeAtom( 0, "ENDSEC"); // end of BLOCKS section

    // end of file
    writeAtom( 0, "EOF");
    _out.close();
}


/** Write a single 2D DXF developed panel draw and cut edges to the file output stream.
 *
 * @param panel = the number of the panel to write
 */
void CSailDxfWriter2d::writePanel(unsigned int panel)
{
    //writeAtom(999,_sail.panel[panel].label.name);

    CSide top = _sail.panel[panel].top;
    CSide btm = _sail.panel[panel].bottom;
    CSide left = _sail.panel[panel].left;
    CSide right = _sail.panel[panel].right;

    CSide ctop = _sail.panel[panel].cutTop;
    CSide cbtm = _sail.panel[panel].cutBottom;
    CSide cleft = _sail.panel[panel].cutLeft;
    CSide cright = _sail.panel[panel].cutRight;

    CPoint3d pt;
    CVector3d V;
    unsigned int i=0, layer=panel+1;
    int j=0;

    //// polyline header for draw line
    writePolyline(layer, 5); // color 5=blue

    // left edge
    pt = left.point[0];
    writeVertex(pt, layer);

    for (i = 1; i < left.nbpoints(); i++)
    {
        V= left.point[i] - pt;
        if (V.norm()> EPS)
        {
            pt = left.point[i];
            writeVertex(pt, layer);
        }
    }

    // panel top edge
    for (i = 0; i < top.nbpoints(); i++)
    {
        V = top.point[i] - pt;
        if (V.norm() > EPS)
        {
            pt = top.point[i];
            writeVertex(pt, layer);
        }
    }

    // panel right edge
    for (j = right.nbpoints()-1; j > - 1; j--)
    {
        V = right.point[j] - pt;
        if (V.norm() > EPS)
        {
            pt = right.point[j];
            writeVertex(pt, layer);
        }
    }

    // panel bottom edge
    for (j = btm.nbpoints()-1; j > -1; j--)
    {
        V = btm.point[j] - pt;
        if (V.norm() > EPS)
        {
            pt = btm.point[j];
            writeVertex(pt, layer);
        }
    }
    // close the circuit to start point
    pt = left.point[0];
    writeVertex(pt, layer);

    writeAtom(0,"SEQEND"); // end draw line

    //// polyline header for cut line
    writePolyline(layer, 1); // color 1=red

    // left edge
    pt = cleft.point[0];
    writeVertex(pt, layer);

    for (i = 1; i < left.nbpoints(); i++)
    {
        V= cleft.point[i] - pt;
        if (V.norm()> EPS)
        {
            pt = cleft.point[i];
            writeVertex(pt, layer);
        }
    }

    // panel top edge
    for (i = 0; i < top.nbpoints(); i++)
    {
        V = ctop.point[i] - pt;
        if (V.norm() > EPS)
        {
            pt = ctop.point[i];
            writeVertex(pt, layer);
        }
    }

    // panel right edge
    for (j = right.nbpoints()-1; j > - 1; j--)
    {
        V = cright.point[j] - pt;
        if (V.norm() > EPS)
        {
            pt = cright.point[j];
            writeVertex(pt, layer);
        }
    }

    // panel bottom edge
    for (j = btm.nbpoints()-1; j > -1; j--)
    {
        V = cbtm.point[j] - pt;
        if (V.norm() > EPS)
        {
            pt = cbtm.point[j];
            writeVertex(pt, layer);
        }
    }
    // close the circuit to start point
    pt = cleft.point[0];
    writeVertex(pt, layer);

    writeAtom(0,"SEQEND"); // end cut line
}



/***********************************
 
           3D DXF export
 
***********************************/


/** Writes a 3D CSail to a 3D DXF file.
 *
 * @param filename the file to write to
 */
void CSailDxfWriter3d::write(const QString &filename)
{
    _out.open(QFile::encodeName(filename), ios::out);
    if (!_out.is_open())
        throw CException("CSailDxfWriter3d::write : unable to write to specified DXF file");

    // write comment
    writeAtom(999,"DXF 3d created by Sailcut CAD");

    // write header section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "HEADER");
    writeAtom( 0, "ENDSEC");

    // write tables section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "TABLES");
    writeAtom( 0, "TABLE");
    writeAtom( 2, "LAYER");
    writeAtom(70, "6");
    for (unsigned int pn = 0; pn < _sail.panel.size(); pn++)
    {
        //writeAtom(999,_sail.panel[pn].label.name);
        writeAtom( 0, "LAYER");
        writeAtom( 2, QString::number(pn+1));
        // flags
        writeAtom(70, "64");
        // colour
        if (pn%2)
            writeAtom(62, "2"); // 1=red 2=yellow
        else
            writeAtom(62, "4"); // 4=cyan 5=blue

        writeAtom( 6, "CONTINUOUS");
    }

    writeAtom( 0, "ENDTAB");
    writeAtom( 0, "ENDSEC");

    // write entities section
    writeAtom( 0, "SECTION");
    writeAtom( 2, "ENTITIES");

    // loop over panels ////////
    for (unsigned int i = 0; i < _sail.panel.size(); i++)
        writePanel(i);

    writeAtom( 0, "ENDSEC");

    // end of file
    writeAtom( 0, "EOF");
    _out.close();
}


/** Write a single 3D DXF panel to the file output stream.
 *
 * @param panel the number of the panel to write
 */
void CSailDxfWriter3d::writePanel(unsigned int panel)
{
    //writeAtom(999,_sail.panel[pn].label.name);
    //writeAtom(999, QString("panel : ") + QString::number(panel));
    CSide top = _sail.panel[panel].top;
    CSide btm = _sail.panel[panel].bottom;
    CSide left = _sail.panel[panel].left;
    CSide right = _sail.panel[panel].right;
    CPoint3d pt;
    unsigned int i=0, layer=panel+1;

    // left triangle fan
    pt = (left.point[0]+left.point[left.nbpoints()-1])*0.5;
    for (i = 1; i < left.nbpoints(); i++)
        writeFace(pt,left.point[i-1],left.point[i],layer);

    // panel triangle strip
    for (i = 1; i < top.nbpoints(); i++)
    {
        writeFace(top.point[i-1],btm.point[i-1],top.point[i],layer);
        writeFace(top.point[i],btm.point[i],btm.point[i-1],layer);
    }

    // right triangle fan
    pt = (right.point[0]+right.point[right.nbpoints()-1])*0.5;
    for (i = 1; i < right.nbpoints(); i++)
        writeFace(pt,right.point[i-1],right.point[i],layer);
}
