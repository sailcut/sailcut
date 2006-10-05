/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: sailwriter-hand.cpp,v 1.7 2006/01/25 21:54:02 jeremy_laine Exp $
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

#include "sailwriter-hand.h"


/** The constructor.
 *
 * @param sail the sail to write
 */
CSailWriterHand::CSailWriterHand(const CSail &sail)
        : TxtWriterTempl<CSail>(sail, sail.sailID)
{}


/** Write sail to TXT "hand" format.
  *
  * @param filename the file to write to
  */
void CSailWriterHand::write(const QString &filename)
{
    // open the output file
    ofstream myOut;
    myOut.open(QFile::encodeName(filename), ios::out);
    if (!myOut.is_open())
        throw CException("CSailWriter::write : unable to write to specified file");

    // write the name of the sail
    myOut << string(_name.toLocal8Bit()) << endl;

    // TODO : modify code to write actual hand output
    //
    // _obj is the sail, loop over its panels
    for(unsigned int i=0; i < _obj.nbpanels(); i++)
    {
        myOut << "===== CPanel : " << i << " ====" << endl;
        myOut << _obj.panel[i].label;
        myOut << "== CSide : left ==" << endl << _obj.panel[i].left;
        myOut << "== CSide : top ==" << endl << _obj.panel[i].top;
        myOut << "== CSide : right ==" << endl << _obj.panel[i].right;
        myOut << "== CSide : bottom ==" << endl << _obj.panel[i].bottom;

        if (_obj.panel[i].hasHems)
        {
            myOut << "== CSide : cutLeft ==" << endl << _obj.panel[i].cutLeft;
            myOut << "== CSide : cutTop ==" << endl << _obj.panel[i].cutTop;
            myOut << "== CSide : cutRight ==" << endl << _obj.panel[i].cutRight;
            myOut << "== CSide : cutBottom ==" << endl << _obj.panel[i].cutBottom;
        }
    }

    myOut.close();
}
