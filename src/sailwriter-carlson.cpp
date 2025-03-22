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

#include "sailwriter-carlson.h"
#include <fstream>
#include <iomanip>
#include <QFile>

#define CRLF "\r\n"

/** Write the draw message
 *
 * @param out the output stream
 * @param ct number of points to be written
 */
void CSailCarlsonWriter::writeDraw(std::ofstream &out, unsigned int ct) const
{
    out.setf(std::ios::left, std::ios::adjustfield);
    out<< std::setw(16) <<"draw"<<  ct << CRLF;
}


/** Write the cut message
 *
 * @param out the output stream
 * @param ct number of points to be written
 */
void CSailCarlsonWriter::writeCut(std::ofstream &out, unsigned int ct) const
{
    out.setf(std::ios::left, std::ios::adjustfield);
    out<< std::setw(16) <<"cut"<<  ct << CRLF;
}


/** Write a point
 *
 * @param out the output stream
 * @param p0 3d point to be written
 */
void CSailCarlsonWriter::writePoint(std::ofstream &out, CPoint3d p0) const
{
    real x=0, y=0;
    x= p0.x();
    y= p0.y();

    out.setf(std::ios::left, std::ios::adjustfield);
    out << std::setw(16) << x << y << CRLF;
}


/** Write panel header to Carlson plotter format
 *
 * @param out the output stream
 * @param panel the number of the panel to write
 *
 */
void CSailCarlsonWriter::writePanelHeader(std::ofstream &out, const CPanel &panel) const
{
    //char identity;
    //identity = panel.label.name;
    unsigned int pencolor = panel.label.color;
    unsigned int htx = panel.label.height;    // text height in mm
    real xoff =0 , yoff = 0, rtx=0; // position and text rotation from x axis.
    xoff= panel.label.origin.x();
    yoff= panel.label.origin.y();
    //rtx = atn2(  xoff= panel.label.direction.y(),  xoff= _sail[panel].label.direction.x());

    out << "panel, "<< panel.label.name <<", "<<pencolor<<", "<< xoff<<", "<<yoff<<", "<<rtx<<", "<<htx << CRLF;
}


/** Write sail to Carlson plotter format.
 *
 * @param sail the sail to write
 * @param filename the file to write to
 */
void CSailCarlsonWriter::write(const CPanelGroup &sail, const QString &filename) const
{
    std::ofstream out;

    out.open(QFile::encodeName(filename), std::ios::out);
    if (!out.is_open())
        throw write_error("CSailCarlsonWriter::write : unable to write to specified file");

    out << "Sailcut Carlson plotter development: Test1" << CRLF;

    unsigned int pn = 0;
    for (pn = 0; pn < sail.size(); pn++)
        writePanel( out, sail[pn] );

    out << "EOF" << CRLF;
    out.close();
}


/** Write panel to Carlson plotter format
 *
 * @param out the output stream
 * @param panel the number of the panel to write
 *
 */
void CSailCarlsonWriter::writePanel(std::ofstream &out, const CPanel &panel) const
{
    CSide top = panel.top;
    CSide btm = panel.bottom;
    CSide left = panel.left;
    CSide right = panel.right;

    CSide ctop = panel.cutTop;
    CSide cbtm = panel.cutBottom;
    CSide cleft = panel.cutLeft;
    CSide cright = panel.cutRight;

    unsigned int i=0; // ascending counter
    int j = 0;  // descending counter

    //// header of panel
    writePanelHeader(out, panel);

    ////  header for draw line
    writeDraw (out, left.size()+top.size()+right.size()+btm.size());

    // left edge
    for (i = 0; i < left.size(); i++)
    {
        writePoint( out, left[i] );
    }

    // panel top edge
    for (i = 0; i < top.size(); i++)
    {
        writePoint( out, top[i] );
    }

    // panel right edge
    for (j = right.size() -1; j> -1; j--)
    {
        writePoint( out, right[j] );
    }

    //// panel bottom edge
    for (j = btm.size() -1; j > -1; j--)
    {
        writePoint( out, btm[j] );
    }

    ////  header for cut line
    writeCut ( out, left.size()+top.size()+right.size()+btm.size() );

    // left edge
    for (i = 0; i < left.size(); i++)
    {
        writePoint( out, cleft[i] );
    }

    // panel top edge
    for (i = 0; i < top.size(); i++)
    {
        writePoint( out, ctop[i] );
    }

    // panel right edge
    for (j = right.size() -1; j > - 1; j--)
    {
        writePoint( out, cright[j] );
    }

    // panel bottom edge
    for (j = btm.size() -1; j > -1; j--)
    {
        writePoint( out, cbtm[j] );
    }
}

