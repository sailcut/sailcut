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

#ifndef XMLREADER_H
#define XMLREADER_H

#include <QMessageBox>
#include "fileio.h"
#include "saildoc.h"

/** This class allows you to read sailcut objects from
 *  an XML file.
 *
 *  @see CPanelGroupXmlReader
 *  @see CSailDefXmlReader
 */
template <class objtype>
class XmlReaderTempl : public CFileIO
{
protected:
    /** the object's name */
    QString _name;

public:
    /** The constructor.
     *
     * @param name the object's name
     * @param ext the file extension to show (defaults to ".xml")
     * @param desc a description of the file type (defaults to "XML files")
     */
    XmlReaderTempl(const QString &name, const QString ext = ".xml",
                   const QString desc = "XML files")
            : CFileIO(ext, desc), _name(name)
    {}
    ;

    /** Display a dialog then read file.
     *
     * @param dest the object we read to
     * @param filename initial file name
     */
    virtual QString readDialog(objtype &dest, const QString &filename = "") const
    {
        QString newfilename = showDialogRead(filename);

        if (!newfilename.isNull())
        {
            try
            {
                dest = read(newfilename);
            }
            catch (CException e)
            {
                QMessageBox::information(0,tr("error"), tr("There was an error reading from the selected file."));
                newfilename = QString::null;
            }
        }
        return newfilename;
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

// some typedefs

/** A class used to read a CPanelGroup from an XML file. */
class CPanelGroupXmlReader : public XmlReaderTempl<CPanelGroup>
{
public:
    /** The constructor.
     *
     * @param name the sail's name
     */
    CPanelGroupXmlReader(const QString &name)
            : XmlReaderTempl<CPanelGroup>(name, ".panel3d", "3D sails or hulls")
    {}
    ;
};


/** A class used to read a CHullDef hull definition from an XML file. */
class CHullDefXmlReader : public XmlReaderTempl<CHullDef>
{
public:
    /** The constructor.
     *
     * @param name the hull definition's name
     */
    CHullDefXmlReader()
            : XmlReaderTempl<CHullDef>("hulldef", ".hulldef", "Hull definitions")
    {}
    ;
};


/** A class used to read a CSailDef sail definition from an XML file. */
class CSailDefXmlReader : public XmlReaderTempl<CSailDef>
{
public:
    /** The constructor.
     *
     * @param name the sail definition's name
     */
    CSailDefXmlReader()
            : XmlReaderTempl<CSailDef>("saildef", ".saildef", "Sail definitions")
    {}
    ;
};


/** A class used to read a CRigDef rig definition from an XML file. */
class CRigDefXmlReader : public XmlReaderTempl<CRigDef>
{
public:
    /** The constructor.
     *
     * @param name the rig definition's name
     */
    CRigDefXmlReader()
            : XmlReaderTempl<CRigDef>("rigdef", ".rigdef", "Rig definitions")
    {}
    ;
};


/** A class used to read a CPrefs set of preferences from an XML file. */
typedef XmlReaderTempl<CPrefs> CPrefsXmlReader;

#endif
