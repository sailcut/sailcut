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

#ifndef SAILDOC_H
#define SAILDOC_H

#include <QDomDocument>
#include "filewriter.h"
#include "panelgroup.h"
#include "boatdef.h"
#include "hulldef.h"
#include "rigdef.h"
#include "saildef.h"
#include "prefs.h"

class doc_element_error : public runtime_error
{
public:
    doc_element_error(const string &message) : runtime_error(message)
    {
        cout << what() << endl;
    }
};

/** @defgroup FileIo File input and output
*/

/** This class handles input and output to Sailcut XML documents.
 *
 * @ingroup FileIo
 */
class CSailDoc : protected QDomDocument
{
public:
    CSailDoc();
    CSailDoc( const QString &filename );

    /** Creates an element of the given type. */
    QDomElement createElement(const QString &type)
    {
        return QDomDocument::createElement(type);
    };
    QDomElement createElement(const QString &type, const QString &name);
    QDomElement createElement(const QString &type, const QString &name, const QString& value);
    QDomElement findElement(const QDomNode &parent, const QString &type, const QString &name);

    // input
    void get
    (const QDomNode &parent, int &i, const QString &name );
    void get
    (const QDomNode &parent, unsigned int &i, const QString &name );
    void get
    (const QDomNode &parent, real &r, const QString &name );
    void get
    (const QDomNode &parent, string &s, const QString &name );
    void get
    (const QDomNode &parent, QString &s, const QString &name );
    void get
    (const QDomNode &parent, CPoint3d &p, const QString &name );
    void get
    (const QDomNode &parent, enumBoatElementType &t, const QString &name );
    void get
    (const QDomNode &parent, enumPanelGroupType &t, const QString &name );
    void get
    (const QDomNode &parent, enumSailCut &c, const QString &name );
    void get
    (const QDomNode &parent, enumSailType &t, const QString &name );
    void get
    (const QDomNode &parent, CHullDef &d, const QString &name );
    void get
    (const QDomNode &parent, CSailDef &d, const QString &name );
    void get
    (const QDomNode &parent, CSide &s, const QString &name );
    void get
    (const QDomNode &parent, CPanel &p, const QString &name );
    void get
    (const QDomNode &parent, CBoatDef &d, const QString &name );
    void get
    (const QDomNode &parent, CBoatElement &s, const QString &name );
    void get
    (const QDomNode &parent, CPanelGroup &g, const QString &name );
    void get
    (const QDomNode &parent, CProfile &p, const QString &name );
    void get
    (const QDomNode &parent, CSailMould &m, const QString &name );
    void get
    (const QDomNode &parent, CPrefs &p, const QString &name );
    void get
    (const QDomNode &parent, CRigDef &d, const QString &name );

    // output
    void put(QDomNode &parent, const int &i, const QString &name ="" );
    void put(QDomNode &parent, const unsigned int &i, const QString &name ="" );
    void put(QDomNode &parent, const real &r, const QString &name ="" );
    void put(QDomNode &parent, const string &s, const QString &name="" );
    void put(QDomNode &parent, const QString &s, const QString &name="" );
    void put(QDomNode &parent, const CPoint3d &p, const QString &name="" );
    void put(QDomNode &parent, const enumBoatElementType &t, const QString &name="");
    void put(QDomNode &parent, const enumPanelGroupType &t, const QString &name="" );
    void put(QDomNode &parent, const enumSailCut &c, const QString &name="" );
    void put(QDomNode &parent, const enumSailType &t, const QString &name="" );
    void put(QDomNode &parent, const CBoatDef &d, const QString &name="" );
    void put(QDomNode &parent, const CBoatElement &s, const QString &name="" );
    void put(QDomNode &parent, const CHullDef &d, const QString &name="" );
    void put(QDomNode &parent, const CSailDef &d, const QString &name="" );
    void put(QDomNode &parent, const CSide &s, const QString &name="" );
    void put(QDomNode &parent, const CPanel &p, const QString &name="" );
    void put(QDomNode &parent, const CPanelGroup &g, const QString &name="" );
    void put(QDomNode &parent, const CProfile &p, const QString &name="" );
    void put(QDomNode &parent, const CSailMould &m, const QString &name="" );
    void put(QDomNode &parent, const CPrefs &p, const QString &name="" );
    void put(QDomNode &parent, const CRigDef &d, const QString &name="" );

    void toFile(const QString &filename);

    /** the toplevel element */
    QDomElement top;

protected:
    /** Reads a vector of elements from an XML document.
     *
     * @param parent the parent node
     * @param v the vector
     * @param name the name of the vector
     */
    template<class myType>
    void get_vector
    (QDomNode &parent, vector<myType>& v, const QString &name="")
    {
        QDomElement e = findElement( parent, "vector", name);
        if ( e.isNull() )
            throw doc_element_error(QString("CSailDoc::get(vector, " + name + ") : did not find requested element").toStdString());

        unsigned int size =   e.attributes().namedItem("size").nodeValue().toInt();
        v.resize(size);

        for ( unsigned int i = 0; i < size; i++)
        {
            get
            (e, v[i], QString::number(i));
        }
    }

    /** Writes a vector of elements to an XML document.
     *
     * @param parent the parent node
     * @param v the vector
     * @param name the name of the vector
     */
    template<class myType>
    void put_vector(QDomNode &parent, const vector<myType>& v, const QString &name="")
    {
        QDomElement e = createElement("vector",name);
        e.setAttribute("size", (unsigned int)v.size());
        parent.appendChild(e);

        for (unsigned int i =0; i<v.size(); i++)
            put(e, v[i], QString::number(i));
    }
};


#endif

