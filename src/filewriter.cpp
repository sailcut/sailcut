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

#include "filewriter.h"
#include <QMessageBox>
#include "geocpp/core.h"

using namespace std;

/** Opens of a dialog to ask for a filename
 *  then writes to a file.
 *
 *  @param filename The filename to start off with (default = "")
 */
QString CFileWriter::writeDialog(const QString &filename)
{
    QString newfilename = showDialogWrite(filename);

    if (!newfilename.isNull())
    {
        try
        {
            write(newfilename);
        }
        catch (CException e)
        {
            QMessageBox::information(0,tr("error"), tr("There was an error writing to the selected file."));
            newfilename = QString::null;
        }
    }
    return newfilename;
}
