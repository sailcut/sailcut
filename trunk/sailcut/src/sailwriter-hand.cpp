/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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

#include "sailwriter-hand.h"
#include <fstream>
#include <QFile>


/** Write sail to TXT "hand" format.
  *
  * @param filename the file to write to
  */
void CSailHandWriter::write(const CPanelGroup &sail, const QString &filename) const
{
    // open the output file
    ofstream myOut;
    myOut.open(QFile::encodeName(filename), ios::out);
    if (!myOut.is_open())
        throw CException("CSailWriter::write : unable to write to specified file");

    // write the name of the sail
    myOut << string(sail.title.toLocal8Bit()) << endl;

    // TODO : modify code to write actual hand output
    //
    // sail is the sail, loop over its panels
    for(unsigned int i=0; i < sail.panel.size(); i++)
    {
        myOut << "===== CPanel : " << i << " ====" << endl;
        myOut << sail.panel[i].label;
        myOut << "== CSide : left ==" << endl << sail.panel[i].left;
        myOut << "== CSide : top ==" << endl << sail.panel[i].top;
        myOut << "== CSide : right ==" << endl << sail.panel[i].right;
        myOut << "== CSide : bottom ==" << endl << sail.panel[i].bottom;

        if (sail.panel[i].hasHems)
        {
            myOut << "== CSide : cutLeft ==" << endl << sail.panel[i].cutLeft;
            myOut << "== CSide : cutTop ==" << endl << sail.panel[i].cutTop;
            myOut << "== CSide : cutRight ==" << endl << sail.panel[i].cutRight;
            myOut << "== CSide : cutBottom ==" << endl << sail.panel[i].cutBottom;
        }
    }

    myOut.close();
}

