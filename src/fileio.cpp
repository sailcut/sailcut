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

#include "fileio.h"
#include <QFileDialog>

using namespace std;

/** Opens a dialog prompting the user for a file to read from.
 *
 * @param filename The filename to start off with (default = "")
 */
QString CFileIO::showDialogRead(const QString &filename) const
{
    return QFileDialog::getOpenFileName(0, tr("Open"), QFileInfo(filename).absolutePath(), _desc + " (*" + _ext + ")");
}


/** Opens a dialog prompting the user for a file to write to.
 *
 * @param filename The filename to start off with (default = "")
 */
QString CFileIO::showDialogWrite (const QString &filename) const
{
    QString newfilename = QFileDialog::getSaveFileName(0, tr("Save"), filename, _desc + " (*" + _ext + ")");

    if (!newfilename.isNull() && newfilename.right(_ext.length()).toLower() != _ext)
        newfilename += _ext;

    return newfilename;
}

