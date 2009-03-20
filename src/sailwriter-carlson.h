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

#ifndef SAILWRITER_CARLSON_H
#define SAILWRITER_CARLSON_H

#include "filewriter.h"
#include "sailcpp/panelgroup.h"
#include <fstream>

/** A class for writing to Carlson plotter files.
 *
 * @ingroup FileIo
 */
class CSailCarlsonWriter : public CFileWriter<CPanelGroup>
{
public:
    /** The constructor.
     */
    CSailCarlsonWriter()
            : CFileWriter<CPanelGroup>(".sp4","Carlson plotter files")
    {}
    ;

    void write(const CPanelGroup &sail, const QString &filename) const;
    void writePanel(ofstream &out, const CPanel &panel) const;
    void writePanelHeader(ofstream &out, const CPanel &panel) const;
    void writeDraw(ofstream &out, unsigned int ct) const;
    void writeCut(ofstream &out, unsigned int ct) const;
    void writePoint(ofstream &out, CPoint3d p0) const;

};

#endif

