/*
 * Copyright (C) 1993-2006 Robert & Jeremy Lain�
 * See AUTHORS file for a full list of contributors.
 * 
 * $Id: sailwriter-txt.h,v 1.6 2006/01/25 21:54:02 jeremy_laine Exp $
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

#ifndef TXTWRITER_H
#define TXTWRITER_H

#include "filewriter.h"
#include <fstream>
#include <QFile>

/** This class allows you to write sailcut objects to
 *  a TXT file.
 *
 *  @see CSailTxtWriter
 */
template <class objtype>
class TxtWriterTempl : public CFileWriter
{
protected :
    /** the object to write */
    objtype _obj;

public:
    /** The constructor.
     *
     * @param obj the object to write
     * @param name the object's name
     */
    TxtWriterTempl(const objtype &obj, const QString &name)
            : CFileWriter(name,".txt","Text files"), _obj(obj)
    {}
    ;

    /** The destructor.
     */
    virtual ~TxtWriterTempl()
    {}
    ;

    /** Write object to TXT format.
     *
     * @param filename the file to write to
     */
    virtual void write(const QString &filename)
    {
        ofstream myOut;
        myOut.open(QFile::encodeName(filename), ios::out);
        if (!myOut.is_open())
            throw CException("TxtWriterTempl::write : unable to write to specified file");
        myOut << string(_name.toLocal8Bit()) << endl;
        myOut << _obj;
        myOut.close();
    }
    ;
};


/** A class used to output a CSail to a TXT file.
 */
class CSailTxtWriter : public TxtWriterTempl<CSail>
{
public:
    /** The constructor.
     *
     * @param sail the sail to write
     */
    CSailTxtWriter(const CSail &sail)
            : TxtWriterTempl<CSail>(sail, sail.sailID)
    {}
    ;
};

#endif
