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

#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "fileio.h"
#include "geocpp/core.h"
#include <QMessageBox>

/** This class is used as the base for various file output
 *  modules requiring a "Save to" dialog.
 */
template <class objtype>
class CFileWriter : public CFileIO
{
public:
    /** The constructor.
     *
     * @param ext the file extension
     * @param desc description of the file type
     */
    CFileWriter(const QString &ext, const QString &desc)
            : CFileIO(ext, desc)
    {}
    ;

    /** Perform the actual writing operation, must be overriden.
     */
    virtual void write(const objtype &, const QString &) = 0;

    /** Opens of a dialog to ask for a filename
     *  then writes to a file.
     *
     *  @param obj The object to write.
     *  @param filename The filename to start off with (default = "")
     */
    virtual QString writeDialog(const objtype &obj, const QString &filename = QString::null)
    {
        QString newfilename = showDialogWrite(filename);
    
        if (!newfilename.isNull())
        {
            try
            {
                write(obj, newfilename);
            }
            catch (CException e)
            {
                QMessageBox::information(0,tr("error"), tr("There was an error writing to the selected file."));
                newfilename = QString::null;
            }
        }
        return newfilename;
    };
};

#endif
