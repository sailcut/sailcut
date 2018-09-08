/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SAILWRITER_XML_H
#define SAILWRITER_XML_H

#include "filewriter.h"
#include "saildoc.h"


/** This class allows you to write sailcut objects to
 *  an XML file.
 *
 *  @ingroup FileIo
 *  @see CPanelGroupXmlWriter
 *  @see CSailDefXmlWriter
 */
template <class objtype>
class XmlWriterTempl : public CFileWriter<objtype>
{
protected :
    /** the object's name */
    QString _name;

public:
    /** The constructor.
     *
     * @param name the object's name
     * @param ext the file extension to show (defaults to ".xml")
     * @param desc a description of the file type (defaults to "XML files")
     */
    XmlWriterTempl(const QString &name,
                   const QString ext = ".xml",
                   const QString desc = "XML files")
            : CFileWriter<objtype>(ext, desc), _name(name)
    {}
    ;

    /** Write object to XML format.
     *
     * @param obj the object to write
     * @param filename the file to write to
     */
    virtual void write(const objtype &obj, const QString &filename) const
    {
        CSailDoc doc;
        doc.put(doc.top, obj, _name);
        doc.toFile(filename);
    };

    /** Read object from an XML file.
     *
     * @param filename the file we read from
     */
    const objtype read(const QString &filename) const
    {
        CSailDoc doc(filename);
        objtype obj;
        doc.get(doc.top, obj, _name);
        return obj;
    };

};


/** A class used to output a CPanelGroup to an XML file. */
class CPanelGroupXmlWriter : public XmlWriterTempl<CPanelGroup>
{
public:
    /** The constructor.
     */
    CPanelGroupXmlWriter()
            : XmlWriterTempl<CPanelGroup>("panelgroup", ".panel3d", "3D sails or hulls")
    {}
    ;
};


/** A class used to output a CHullDef to an XML file. */
class CHullDefXmlWriter : public XmlWriterTempl<CHullDef>
{
public:
    /** The constructor.
     */
    CHullDefXmlWriter()
            : XmlWriterTempl<CHullDef>("hulldef", ".hulldef", "Hull definitions")
    {}
    ;
};


/** A class used to output a CSailDef to an XML file. */
class CSailDefXmlWriter : public XmlWriterTempl<CSailDef>
{
public:
    /** The constructor.
     */
    CSailDefXmlWriter()
            : XmlWriterTempl<CSailDef>("saildef", ".saildef", "Sail definitions")
    {}
    ;
};


/** A class used to output a CBoatDef to an XML file. */
class CBoatDefXmlWriter : public XmlWriterTempl<CBoatDef>
{
public:
    /** The constructor.
     */
    CBoatDefXmlWriter()
            : XmlWriterTempl<CBoatDef>("boatdef", ".boatdef", "Boat definitions")
    {}
    ;
};


/** A class used to output a CRigDef to an XML file. */
class CRigDefXmlWriter : public XmlWriterTempl<CRigDef>
{
public:
    /** The constructor.
     */
    CRigDefXmlWriter()
            : XmlWriterTempl<CRigDef>("rigdef", ".rigdef", "Rig definitions")
    {}
    ;
};


/** A class used to output a CPrefs to an XML file. */
class CPrefsXmlWriter : public XmlWriterTempl<CPrefs>
{
public:
    /** The constructor.
     */
    CPrefsXmlWriter()
            : XmlWriterTempl<CPrefs>("prefs")
    {}
    ;
};

#endif
