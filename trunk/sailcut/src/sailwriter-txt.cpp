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

#include "sailwriter-txt.h"
#include <fstream>
#include <QFile>


/** Write sail to TXT format.
 *
 * @param sail the panel group to write
 * @param filename the file to write to
 */
void CSailTxtWriter::write(const CPanelGroup &sail, const QString &filename) const
{
    ofstream myOut;
    myOut.open(QFile::encodeName(filename), ios::out);
    if (!myOut.is_open())
        throw write_error("CSailTxtWriter::write : unable to write to specified file");
    myOut << sail.title << endl;
    myOut << sail;
    myOut.close();
}

