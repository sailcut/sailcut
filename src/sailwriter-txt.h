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

#ifndef TXTWRITER_H
#define TXTWRITER_H

#include "filewriter.h"
#include "sail.h"


/** A class used to output a CSail to a TXT file.
 */
class CSailTxtWriter : public CFileWriter
{
protected :
    /** the sail to write */
    CSail _sail;

public:
    /** The constructor.
     *
     * @param sail the sail to write
     */
    CSailTxtWriter(const CSail &sail)
            : CFileWriter(".txt","Text files"), _sail(sail)
    {}
    ;

    virtual void write(const QString &filename);
};


#endif
