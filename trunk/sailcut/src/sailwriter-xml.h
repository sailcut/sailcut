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

#ifndef SAILWRITER_XML_H
#define SAILWRITER_XML_H

#include "filewriter.h"

/** This class allows you to write sailcut objects to
 *  an XML file.
 *
 *  @see CPanelGroupXmlWriter
 *  @see CSailDefXmlWriter
 */
template <class objtype>
class XmlWriterTempl : public CFileWriter
{
protected :
    /** the object's name */
    QString _name;
    /** the object to write */
    objtype _obj;

public:
    /** The constructor.
     *
     * @param obj the object to write
     * @param name the object's name
     * @param ext the file extension to show (defaults to ".xml")
     * @param desc a description of the file type (defaults to "XML files")
     */
    XmlWriterTempl(const objtype &obj, const QString &name,
                   const QString ext = ".xml", const QString desc = "XML files")
            : CFileWriter(ext, desc), _name(name), _obj(obj)
    {}
    ;

    /** Write object to XML format.
     *
     * @param filename the file to write to
     */
    virtual void write(const QString &filename)
    {
        CSailDoc doc;
        doc.put(doc.top, _obj, _name);
        doc.toFile(filename);
    };
};


/** A class used to output a CPanelGroup to an XML file. */
class CPanelGroupXmlWriter : public XmlWriterTempl<CPanelGroup>
{
public:
    /** The constructor.
     *
     * @param sail the sail to write
     * @param name the sail's name
     */
    CPanelGroupXmlWriter(const CPanelGroup &sail, const QString &name)
            : XmlWriterTempl<CPanelGroup>(sail, name, ".panel3d", "3D sails or hulls")
    {}
    ;
};

/** A class used to output a CSailDef to an XML file. */
class CSailDefXmlWriter : public XmlWriterTempl<CSailDef>
{
public:
    /** The constructor.
     *
     * @param saildef the CSailDef to write
     * @param name the CSailDef's name
     */
    CSailDefXmlWriter(const CSailDef &saildef, const QString &name)
            : XmlWriterTempl<CSailDef>(saildef, name, ".saildef", "Sail definitions")
    {}
    ;
};

/** A class used to output a CRigDef to an XML file. */
class CRigDefXmlWriter : public XmlWriterTempl<CRigDef>
{
public:
    /** The constructor.
     *
     * @param rigdef the CRigDef to write
     * @param name the CRigDef's name
     */
    CRigDefXmlWriter(const CRigDef &rigdef, const QString &name)
            : XmlWriterTempl<CRigDef>(rigdef, name, ".sailrig", "Rig definitions")
    {}
    ;
};

/** A class used to output a CPrefs to an XML file. */
typedef XmlWriterTempl<CPrefs> CPrefsXmlWriter;


#endif
