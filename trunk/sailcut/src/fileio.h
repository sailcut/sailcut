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

#ifndef FILEIO_H
#define FILEIO_H

#ifdef WIN32
  #define CRLF   endl
#else
  #define CRLF "\r\n"
#endif

#include <QObject>

/** This is a generic class used as the base for various file
 *  input and output modules.
 */
class CFileIO : public QObject
{
public:
    /** The constructor.
     *
     * @param name the object's name
     * @param ext the file extension
     * @param desc description of the file type
     */
    CFileIO(const QString &name, const QString &ext, const QString &desc) : _name(name), _ext(ext), _desc(desc)
    {}
    ;

    virtual QString showDialogWrite(const QString &filename) const;
    virtual QString showDialogRead(const QString &filename) const;

protected:
    /** the object's name */
    QString _name;
    /** file extension */
    QString _ext;
    /** description of the file type */
    QString _desc;
};

#endif
