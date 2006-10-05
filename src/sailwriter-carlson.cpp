/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: sailwriter-carlson.cpp,v 1.11 2006/01/25 21:54:02 jeremy_laine Exp $
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

#include "sailwriter-carlson.h"
#include <fstream>
#include <iomanip>
#include <QFile>


/** The constructor.
 * @param sail the sail to write
 */
CSailCarlsonWriter::CSailCarlsonWriter(const CSail &sail)
        : CFileWriter(sail.sailID,".sp4","Carlson plotter files"), _sail(sail)
{}

/** Write the draw message
 *
 * @param ct number of points to be written
 */
void CSailCarlsonWriter::writeDraw(unsigned int ct)
{
    _myOut.setf(ios::left, ios::adjustfield);
    _myOut<< setw(16) <<"draw"<<  ct << CRLF;
}


/** Write the cut message
 *
 * @param ct number of points to be written
 */
void CSailCarlsonWriter::writeCut(unsigned int ct)
{
    _myOut.setf(ios::left, ios::adjustfield);
    _myOut<< setw(16) <<"cut"<<  ct << CRLF;
}


/** Write a point
 *
 * @param p0 3d point to be written
 */
void CSailCarlsonWriter::writePoint(CPoint3d p0)
{
    real x=0, y=0;
    x= p0.x();
    y= p0.y();

    _myOut.setf(ios::left, ios::adjustfield);
    _myOut << setw(16) << x << y << CRLF;
}


/** Write the end of file mark
 */
void CSailCarlsonWriter::writeEOF()
{
    _myOut << "EOF" << CRLF;
}


/** Write panel header to Carlson plotter format
 *
 *
 * @param panel the number of the panel to write
 *
 */
void CSailCarlsonWriter::writePanelHeader(unsigned int panel)
{
    //char identity;
    //identity = _sail.panel[panel].label.name;
    unsigned int pencolor = _sail.panel[panel].label.color;
    unsigned int htx = _sail.panel[panel].label.height;    // text height in mm
    real xoff =0 , yoff = 0, rtx=0; // position and text rotation from x axis.
    xoff= _sail.panel[panel].label.origin.x();
    yoff= _sail.panel[panel].label.origin.y();
    //rtx = atn2(  xoff= _sail.panel[panel].label.direction.y(),  xoff= _sail.panel[panel].label.direction.x());

    _myOut << "panel, "<< string(_sail.panel[panel].label.name.toLocal8Bit()) <<", "<<pencolor<<", "<< xoff<<", "<<yoff<<", "<<rtx<<", "<<htx << CRLF;
}


/** Write sail to Carlson plotter format.
 *
 * @param filename the file to write to
 */
void CSailCarlsonWriter::write(const QString &filename)
{
    // ofstream myOut;

    _myOut.open(QFile::encodeName(filename),ios::out);
    if (!_myOut.is_open())
        throw CException("CSailCarlsonWriter::write : unable to write to specified file");

    _myOut << "Sailcut Carlson plotter development: Test1" << CRLF;

    unsigned int pn = 0;
    for (pn = 0; pn < _sail.nbpanels(); pn++)
        writePanel(pn);

    writeEOF();
    _myOut.close();
}


/** Write panel to Carlson plotter format
 *
 *
 * @param panel the number of the panel to write
 *
 */
void CSailCarlsonWriter::writePanel(unsigned int panel)
{

    CSide top = _sail.panel[panel].top;
    CSide btm = _sail.panel[panel].bottom;
    CSide left = _sail.panel[panel].left;
    CSide right = _sail.panel[panel].right;

    CSide ctop = _sail.panel[panel].cutTop;
    CSide cbtm = _sail.panel[panel].cutBottom;
    CSide cleft = _sail.panel[panel].cutLeft;
    CSide cright = _sail.panel[panel].cutRight;

    unsigned int i=0; // ascending counter
    int j = 0;  // descending counter

    //// header of panel
    writePanelHeader(panel);

    ////  header for draw line
    writeDraw (left.nbpoints()+top.nbpoints()+right.nbpoints()+btm.nbpoints());

    // left edge
    for (i = 0; i < left.nbpoints(); i++)
    {
        writePoint( left.point[i] );
    }

    // panel top edge
    for (i = 0; i < top.nbpoints(); i++)
    {
        writePoint( top.point[i] );
    }

    // panel right edge
    for (j = right.nbpoints() -1; j> -1; j--)
    {
        writePoint( right.point[j] );
    }

    //// panel bottom edge
    for (j = btm.nbpoints() -1; j > -1; j--)
    {
        writePoint( btm.point[j] );
    }

    ////  header for cut line
    writeCut (left.nbpoints()+top.nbpoints()+right.nbpoints()+btm.nbpoints());

    // left edge
    for (i = 0; i < left.nbpoints(); i++)
    {
        writePoint( cleft.point[i] );
    }

    // panel top edge
    for (i = 0; i < top.nbpoints(); i++)
    {
        writePoint( ctop.point[i] );
    }

    // panel right edge
    for (j = right.nbpoints() -1; j > - 1; j--)
    {
        writePoint( cright.point[j] );
    }

    // panel bottom edge
    for (j = btm.nbpoints() -1; j > -1; j--)
    {
        writePoint( cbtm.point[j] );
    }
}

