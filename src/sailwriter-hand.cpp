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

#include "sailwriter-hand.h"
#include <fstream>
#include <QFile>


/** Write sail to TXT "hand" format.
 *
 * @param sail the panel group to write
 * @param filename the file to write to
 */
void CSailHandWriter::write(const CPanelGroup &sail, const QString &filename) const
{
    // open the output file
    std::ofstream myOut;
    myOut.open(QFile::encodeName(filename), std::ios::out);
    if (!myOut.is_open())
        throw write_error("CSailWriter::write : unable to write to specified file");

    // write the name of the sail
    myOut << sail.title << std::endl;

    // TODO : modify code to write actual hand output
    //
    // sail is the sail, loop over its panels
    for (unsigned int i=0; i < sail.size(); i++)
    {
        myOut << "===== CPanel : " << i << " ====" << std::endl;
        myOut << sail[i].label;
        myOut << "== CSide : left ==" << std::endl << sail[i].left;
        myOut << "== CSide : top ==" << std::endl << sail[i].top;
        myOut << "== CSide : right ==" << std::endl << sail[i].right;
        myOut << "== CSide : bottom ==" << std::endl << sail[i].bottom;

        if (sail[i].hasHems)
        {
            myOut << "== CSide : cutLeft ==" << std::endl << sail[i].cutLeft;
            myOut << "== CSide : cutTop ==" << std::endl << sail[i].cutTop;
            myOut << "== CSide : cutRight ==" << std::endl << sail[i].cutRight;
            myOut << "== CSide : cutBottom ==" << std::endl << sail[i].cutBottom;
        }
    }

    myOut.close();
}

