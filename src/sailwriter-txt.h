/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
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

#ifndef TXTWRITER_H
#define TXTWRITER_H

#include "filewriter.h"
#include "sailcpp/panelgroup.h"


/** A class used to output a CPanelGroup to a TXT file.
 *
 * @ingroup FileIo
 */
class CSailTxtWriter : public CFileWriter<CPanelGroup>
{
public:
    /** The constructor.
     */
    CSailTxtWriter() : CFileWriter<CPanelGroup>(".txt","Text files")
    {}
    ;

    virtual void write(const CPanelGroup &sail, const QString &filename) const;
};


#endif
