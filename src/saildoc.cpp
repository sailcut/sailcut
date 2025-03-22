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

#include "saildoc.h"
#include <QFile>

/** This contains all the input and output functions
 *  to read and write the sail, hull, rig, boat data to file.
 */

/**************************************************************************

                       construction / destruction

**************************************************************************/

/** Constructs an empty CSailDoc object.
 */
CSailDoc::CSailDoc()
        : QDomDocument("Sailcut")
{
    top = createElement("CSailDoc");
    appendChild(top);
}


/** Constructs a CSailDoc from a given XML file.
 *
 * @param filename
 */
CSailDoc::CSailDoc( const QString &filename )
{
    QFile f( filename );

    if ( !f.open(QIODevice::ReadOnly) )
        throw read_error("CSailDoc::CSailDoc : cannot open file for read access!");

    if ( !setContent( &f ) )
    {
        f.close();
        throw read_error("CSailDoc::CSailDoc : cannot set XML content from file!");
    }

    f.close();

    top = documentElement();
}



/** Creates an element with the given name and type.
 *
 * @param type a string containing the type of the element
 * @param name a string containing the name of the element
 */
QDomElement CSailDoc::createElement(const QString &type, const QString &name)
{
    QDomElement e = createElement(type);
    e.setAttribute("name", name);
    return e;
}


/** Creates an element with the given name, type and value.
 *
 * @param type a string containing the type of the element
 * @param name a string containing the name of the element
 * @param value a string containing the name of the element
 */
QDomElement CSailDoc::createElement(const QString &type, const QString &name, const QString &value)
{
    QDomElement e = createElement(type,name);
    e.setAttribute("value", value);
    return e;
}


/** Looks for an element with a given name and type in the current XML document.
 *
 * @param parent the parent node
 * @param type a string containing the type of the element
 * @param name a string containing the name of the element
 */
QDomElement CSailDoc::findElement(const QDomNode &parent, const QString &type, const QString &name)
{
    QDomNamedNodeMap attr;
    QDomNode n = parent.firstChild();

    while ( !n.isNull() )
    {
        if (n.nodeName() == type)
        {
            attr = n.toElement().attributes();
            if (attr.namedItem("name").nodeValue() == name)
                return n.toElement();
        }
        n = n.nextSibling();
    }

    // we didn't find the element, throw an exception
    throw doc_element_error(QString("CSailDoc::findElement(" + type + "," + name + ") : did not find requested element").toStdString());

    n.clear();
    return n.toElement();
}


/**************************************************************************

                                Input

**************************************************************************/

/** Reads an integer value from an XML document.
 *
 * @param parent the parent node
 * @param i the integer
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, int &i, const QString &name )
{
    QDomElement e = findElement( parent, "int", name);
    QDomNamedNodeMap attr = e.attributes();
    i = attr.namedItem("value").nodeValue().toInt();
}


/** Reads an unsigned integer value from an XML document.
 *
 * @param parent the parent node
 * @param i the unsigned integer
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, unsigned int &i, const QString &name )
{
    int temp;
    get(parent, temp, name);
    i = temp;
}


/** Reads a real value from an XML document.
 *
 * @param parent the parent node
 * @param r the real value
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, real &r, const QString &name )
{
    QDomElement e = findElement( parent, "real", name);
    QDomNamedNodeMap attr = e.attributes();
    r = attr.namedItem("value").nodeValue().toDouble();
}


/** Reads a std::string value from an XML document.
 *
 * @param parent the parent node
 * @param s the string
 * @param name the name of the string
 */
void CSailDoc::get
( const QDomNode &parent, std::string &s, const QString &name )
{
    QDomElement e = findElement( parent, "string", name);
    QDomNamedNodeMap attr = e.attributes();
    s = attr.namedItem("value").nodeValue().toStdString();
}


/** Reads a QString string value from an XML document.
 *
 * @param parent the parent node
 * @param s the string
 * @param name the name of the string
 */
void CSailDoc::get
( const QDomNode &parent, QString &s, const QString &name )
{
    QDomElement e = findElement( parent, "string", name);
    QDomNamedNodeMap attr = e.attributes();
    s = attr.namedItem("value").nodeValue();
}


/** Reads a CPoint3D point with x,y,z coordinates from an XML document.
 *
 * @param parent the parent node
 * @param p the 3D point
 * @param name the name of the 3D point
 */
void CSailDoc::get
( const QDomNode &parent, CPoint3d &p, const QString &name )
{
    real x, y, z;
    QDomElement e = findElement( parent, "CPoint3d", name);
    get(e, x, "x");
    get(e, y, "y");
    get(e, z, "z");
    p = CPoint3d(x, y, z);
}


/** Reads an enumPanelGroupType enumerated Panel Group Type from an XML document.
 *
 * @param parent the parent node
 * @param t the enumPanelGroupType
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, enumPanelGroupType &t, const QString &name )
{
    QDomElement e = findElement( parent, "enumPanelGroupType", name);
    QString s = e.attributes().namedItem("value").nodeValue();

    if (!s.compare("SAIL"))
    {
        t = SAIL;
    }
    else if (!s.compare("RIG"))
    {
        t = RIG;
    }
    else if (!s.compare("HULL"))
    {
        t = HULL;
    }
    else
    {
        throw std::invalid_argument("CSailDoc::get : unknown panel group type");
    }
}


/** Reads an enumSailCut enumerated Sail Cut type from an XML document.
 *
 * @param parent the parent node
 * @param c the enumSailCut
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, enumSailCut &c, const QString &name )
{
    QDomElement e = findElement( parent, "enumSailCut", name);
    QString s = e.attributes().namedItem("value").nodeValue();

    if (!s.compare("CROSS"))
        c = CROSS;
    if (!s.compare("TWIST"))
        c = TWIST;
    if (!s.compare("HORIZONTAL"))
        c = HORIZONTAL;
    if (!s.compare("VERTICAL"))
        c = VERTICAL;
    if (!s.compare("RADIAL"))
        c = RADIAL;
    if (!s.compare("MITRE"))
        c = MITRE;
    if (!s.compare("MITRE2"))
        c = MITRE2;
}


/** Reads an enumSailType enumerated Sail Type from an XML document.
 *
 * @param parent the parent node
 * @param t the enumSailType
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, enumSailType &t, const QString &name )
{
    QDomElement e = findElement( parent, "enumSailType", name);
    QString s = e.attributes().namedItem("value").nodeValue();

    if (!s.compare("MAINSAIL"))
        t = MAINSAIL;
    if (!s.compare("JIB"))
        t = JIB;
    if (!s.compare("WING"))
        t = WING;
}


/** Reads an enumBoatElementType enumerated Boat Element Type from an XML document.
 *
 * @param parent the parent node
 * @param t the enumBoatElementType
 * @param name the name of the value
 */
void CSailDoc::get
( const QDomNode &parent, enumBoatElementType &t, const QString &name )
{
    QDomElement e = findElement( parent, "enumBoatElementType", name);
    QString s = e.attributes().namedItem("value").nodeValue();

    if (!s.compare("HULLDEF"))
        t = HULLDEF;
    if (!s.compare("SAILDEF"))
        t = SAILDEF;
    if (!s.compare("RIGDEF"))
        t = RIGDEF;
    if (!s.compare("PANELGROUP"))
        t = PANELGROUP;
}


/** Reads a CHullDef hull definition from an XML document.
 *
 * @param parent the parent node
 * @param d the hull definition
 * @param name the name of the hull definition
 */
void CSailDoc::get
( const QDomNode &parent, CHullDef &d, const QString &name )
{
    QDomElement e = findElement( parent, "CHullDef", name);

    get(e, d.hullID, "hullID");

    /** read hull bottom data */
    get(e, d.BLWL, "BLWL");
    get(e, d.BfwdHeight, "BfwdHeight");
    get(e, d.BaftHeight, "BaftHeight");
    get(e, d.BSlopeA, "BSlopeA");
    get(e, d.BBW, "BBW");
    get(e, d.BBWPos, "BBWPos");
    get(e, d.BaftW, "BaftW");
    get(e, d.BDeadriseA, "BDeadriseA");
    get(e, d.BSweepA, "BSweepA");
    get(e, d.BfwdShape, "BfwdShape");
    get(e, d.BaftShape, "BaftShape");

    /** read hull deck data */
    get(e, d.DfwdHeight, "DfwdHeight");
    get(e, d.DaftHeight, "DaftHeight");
    get(e, d.DSlopeA, "DSlopeA");

    /** read hull stem angle */
    get(e, d.StemA, "StemA");

    /** read hull transom angle */
    get(e, d.TransomA, "TransomA");

    /** read hull side planks data */
    get(e, d.NBPlank, "NBPlank");
    get(e, d.TopPlankA, "TopPlankA");
    get(e, d.LowPlankA, "lowPlankA");
    // get(e, d.AutoPlank, "AutoPlank");
    //
}


/** Reads a CSailDef sail definition from an XML document.
 *  NEW members are added at the end of the list
 *  See also CSailDoc::put(QDomNode &parent, const CSailDef &d, const QString &name )
 *
 * @param parent the parent node
 * @param d the sail definition
 * @param name the name of the sail definition
 */
void CSailDoc::get
( const QDomNode &parent, CSailDef &d, const QString &name )
{
    QDomElement e = findElement( parent, "CSailDef", name);

    /** read sail cut layout and type */
    get(e, d.sailCut, "sailCut");
    get(e, d.sailType, "sailType");

    /** read sail boat data */
    get(e, d.LOA, "LOA");
    get(e, d.foreI, "foreI");
    get(e, d.foreJ, "foreJ");
    get(e, d.tackX, "tackX");
    get(e, d.tackY, "tackY");

    /** read sail sides */
    get(e, d.luffL, "luffL");
    get(e, d.rake, "rake");
    get(e, d.gaffDeg, "gaffDeg");
    get(e, d.gaffL, "gaffL");
    get(e, d.footL, "footL");
    get(e, d.leechL, "leechL");

    /** read sail shape of sides */
    get(e, d.luffR, "luffR");
    get(e, d.gaffR, "gaffR");
    get(e, d.leechR, "leechR");
    get(e, d.footR, "footR");

    get(e, d.luffRP, "luffRP");
    get(e, d.gaffRP, "gaffRP");
    get(e, d.leechRP, "leechRP");
    get(e, d.footRP, "footRP");

    /** read sail cloth width, seam and hems width */
    get(e, d.clothW, "clothW");
    get(e, d.seamW, "seamW");
    get(e, d.leechHemW, "leechHemW");
    get(e, d.hemsW, "hemsW");

    /** read sail twist */
    get(e, d.twistDeg, "twistDeg");

    /** read sail mould */
    get(e, d.mould, "mould");

    /** read sail sheeting angle */
    get(e, d.sheetDeg, "sheetDeg");

    /** NOTE : we maintain backward file format compatibility by adding below
     * all new members in the order they are introduced in CSailDoc::put
     */
    try
    {
        /** read Sail ID */
        get(e, d.sailID, "sailID");

        /** read Radial sections */
        get(e, d.nbSections, "nbSections");

        /** read Radial gores */
        get(e, d.nbGores, "nbGores");

        /** read Luff gores */
        get(e, d.nbLuffGores, "nbLuffGores");

        /** read Wing dihedral angle */
        get(e, d.dihedralDeg, "dihedralDeg");

        /** read Foot hem width */
        get(e, d.footHemW, "footHemW");

    }
    catch (doc_element_error const&)
    {
        /** catch read error to avoid killing the program */
    }
}


/** Reads a CSide sail side from an XML document.
 *
 * @param parent the parent node
 * @param s the sail side
 * @param name the name of the sail side
 */
void CSailDoc::get
( const QDomNode &parent, CSide &s, const QString &name )
{
    QDomElement e = findElement( parent, "CSide", name);
    get_vector(e, s, "point");
}


/** Reads a CPanel sail panel from an XML document.
 *
 * @param parent the parent node
 * @param p the panel
 * @param name the name of the panel
 */
void CSailDoc::get
( const QDomNode &parent, CPanel &p, const QString &name )
{
    QDomElement e = findElement( parent, "CPanel", name);
    get(e, p.top, "top");
    get(e, p.bottom, "bottom");
    get(e, p.left, "left");
    get(e, p.right, "right");

    int temp;
    get(e, temp, "hasHems");
    p.hasHems = (temp != 0);

    if (p.hasHems)
    {
        get(e, p.cutTop, "cutTop");
        get(e, p.cutBottom, "cutBottom");
        get(e, p.cutLeft, "cutLeft");
        get(e, p.cutRight, "cutRight");
    }

}


/** Reads a CPanelGroup from an XML document.
 *
 * @param parent the parent node
 * @param g the panel group
 * @param name the name of the panel group
 */
void CSailDoc::get
( const QDomNode &parent, CPanelGroup &g, const QString &name )
{
    QDomElement e = findElement( parent, "CPanelGroup", name);
    get_vector(e, g, "panel");
    get(e, g.title, "title");
    get_vector(e, g.child, "child");

    /* NOTE : we maintain backward file format compatibility
     * by adding below all new members in the order they were introduced
     */
    try
    {
        get(e, g.type, "type");
    }
    catch (doc_element_error const&)
    {
        // to avoid killing the program
    }
}


/** Reads a CProfile sail profile from an XML document.
 *
 * @param parent the parent node
 * @param p the profile
 * @param name the name of the profile
 */
void CSailDoc::get
( const QDomNode &parent, CProfile &p, const QString &name )
{
    QDomElement e = findElement( parent, "CProfile", name);
    real depth=0,kluff=0,kleech=0;
    get(e, depth, "depth");
    get(e, kleech, "kleech");
    get(e, kluff, "kluff");

    p = CProfile(depth,kleech,kluff);
}


/** Reads a CBoatDef boat definition from an XML document.
 *
 * @param parent the parent node
 * @param d the boat definition
 * @param name the name of the boat definition
 */
void CSailDoc::get
( const QDomNode &parent, CBoatDef &d, const QString &name )
{
    QDomElement e = findElement( parent, "CBoatDef", name);
    get_vector(e, d, "element");
}


/** Reads a CBoatElement from an XML document.
 *
 * @param parent the parent node
 * @param s the boat element
 * @param name the name of the boat element
 */
void CSailDoc::get
( const QDomNode &parent, CBoatElement &s, const QString &name )
{
    QDomElement e = findElement( parent, "CBoatElement", name);
    get(e, (CPanelGroup&)s, "panelgroup");
    get(e, s.type, "type");
    get(e, s.filename, "filename");
    get(e, s.origin, "origin");
}


/** Reads a CSailMould sail mould from an XML document.
 *
 * @param parent the parent node
 * @param m the mould
 * @param name the name of the mould
 */
void CSailDoc::get
( const QDomNode &parent, CSailMould &m, const QString &name )
{
    QDomElement e = findElement( parent, "CSailMould", name);
    get(e, m.vertpos, "vertpos");
    get_vector(e, m.profile, "profile");
}


/** Reads a CPrefs set of preferences from an XML document.
 *
 * @param parent the parent node
 * @param p the preferences
 * @param name the name of the preferences
 */
void CSailDoc::get
( const QDomNode &parent, CPrefs &p, const QString &name )
{
    QDomElement e = findElement( parent, "CPrefs", name);
    try
    {
        get(e, p.language, "language");
        get_vector(e, p.mruDocuments, "mruDocuments");
        get(e, p.mainWindowHeight, "mainWindowHeight");
        get(e, p.mainWindowWidth, "mainWindowWidth");
    }
    catch (doc_element_error const&)
    {
        // we do not let this kill the program
    }
}


/** Reads a CRigDef rig definition from an XML document.
 *
 * @param parent the parent node
 * @param d the rig definition
 * @param name the name of the rig definition
 */
void CSailDoc::get
( const QDomNode &parent, CRigDef &d, const QString &name )
{
    QDomElement e = findElement( parent, "CRigDef", name);

    get(e, d.rigID, "rigID");

    /** read fore triangle data */
    get(e, d.foreI, "foreI");
    get(e, d.foreJ, "foreJ");
    /** read mast data */
    get(e, d.MHeight, "MHeight");
    get(e, d.MCord, "MCord");
    get(e, d.MWidth, "MWidth");
    get(e, d.MRakeM, "MRakeM");
    get(e, d.MRakeD, "MRakeD");
    get(e, d.MBase, "MBase");
    get(e, d.MRnd, "MRnd");
    get(e, d.MRndPos, "MRndPos");
    /** read shrouds data */
    get(e, d.CSH, "CSH");
    get(e, d.CSB, "CSB");
    get(e, d.LSB, "LSB");
    /** read spreaders data */
    get(e, d.SPNB, "SPNB");
    get(e, d.SPH[0], "SPH0");
    get(e, d.SPH[1], "SPH1");
    get(e, d.SPH[2], "SPH2");
    get(e, d.SPH[3], "SPH3");
    get(e, d.SPW[0], "SPW0");
    get(e, d.SPW[1], "SPW1");
    get(e, d.SPW[2], "SPW2");
    get(e, d.SPW[3], "SPW3");
    /** read mainsail data */
    get(e, d.BAD, "BAD");
    get(e, d.HAD, "HAD");
    //
}


/**************************************************************************

                                Output

**************************************************************************/

/** Puts an integer value to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const int &i, const QString &name )
{
    QDomElement e = createElement("int",name,QString::number(i));
    parent.appendChild(e);
}


/** Puts an unsigned integer value to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const unsigned int &i, const QString &name )
{
    put(parent, int(i), name);
}


/** Puts a real value to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const real &r, const QString &name )
{
    QDomElement e = createElement("real",name, QString::number(r));
    parent.appendChild(e);
}


/** Puts a std::string value to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const std::string &s, const QString &name )
{
    QDomElement e = createElement("string", name, QString::fromStdString(s));
    parent.appendChild(e);
}


/** Puts a QString value to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const QString &s, const QString &name )
{
    QDomElement e = createElement("string", name, s);
    parent.appendChild(e);
}


/** Puts a CPoint3d to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CPoint3d &p, const QString &name )
{
    QDomElement e = createElement("CPoint3d",name);
    parent.appendChild(e);

    put(e, p.x(), "x");
    put(e, p.y(), "y");
    put(e, p.z(), "z");
}


/** Puts an enumBoatElementType enumerated Boat Element Type to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const enumBoatElementType &t, const QString &name )
{
    QString value;
    switch (t)
    {
    case HULLDEF:
        value = "HULLDEF";
        break;
    case SAILDEF:
        value = "SAILDEF";
        break;
    case RIGDEF:
        value = "RIGDEF";
        break;
    case PANELGROUP:
        value = "PANELGROUP";
        break;
    }
    QDomElement e = createElement("enumBoatElementType",name,value);
    parent.appendChild(e);
}


/** Puts an enumPanelGroupType enumerated Panel Group Type to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const enumPanelGroupType &t, const QString &name )
{
    QString value;
    switch (t)
    {
    case SAIL:
        value = "SAIL";
        break;
    case RIG:
        value = "RIG";
        break;
    case HULL:
        value = "HULL";
        break;
    }
    QDomElement e = createElement("enumPanelGroupType",name,value);
    parent.appendChild(e);
}


/** Puts an enumSailCut enumerated Sail Cut Type to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const enumSailCut &c, const QString &name )
{
    QString value;
    switch (c)
    {
    case CROSS:
        value = "CROSS";
        break;
    case TWIST:
        value = "TWIST";
        break;
    case HORIZONTAL:
        value = "HORIZONTAL";
        break;
    case VERTICAL:
        value = "VERTICAL";
        break;
    case RADIAL:
        value = "RADIAL";
        break;
    case MITRE:
        value = "MITRE";
        break;
    case MITRE2:
        value = "MITRE2";
        break;
    }
    QDomElement e = createElement("enumSailCut",name,value);
    parent.appendChild(e);
}


/** Puts an enumSailType enumerated Sailf Type to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const enumSailType &t, const QString &name )
{
    QString value;
    switch (t)
    {
    case MAINSAIL:
        value = "MAINSAIL";
        break;
    case JIB:
        value = "JIB";
        break;
    case WING:
        value = "WING";
        break;
    }
    QDomElement e = createElement("enumSailType",name,value);
    parent.appendChild(e);
}


/** Puts a CHullDef hull definition to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CHullDef &d, const QString &name )
{
    QDomElement e = createElement("CHullDef",name);
    parent.appendChild(e);

    put(e, d.hullID, "hullID");

    /** write hull bottom data */
    put(e, d.BLWL, "BLWL");
    put(e, d.BfwdHeight, "BfwdHeight");
    put(e, d.BaftHeight, "BaftHeight");
    put(e, d.BSlopeA, "BSlopeA");
    put(e, d.BBW, "BBW");
    put(e, d.BBWPos, "BBWPos");
    put(e, d.BaftW, "BaftW");
    put(e, d.BDeadriseA, "BDeadriseA");
    put(e, d.BSweepA, "BSweepA");
    put(e, d.BfwdShape, "BfwdShape");
    put(e, d.BaftShape, "BaftShape");

    /** write hull deck data */
    put(e, d.DfwdHeight, "DfwdHeight");
    put(e, d.DaftHeight, "DaftHeight");
    put(e, d.DSlopeA, "DSlopeA");

    /** write hull stem angle */
    put(e, d.StemA, "StemA");

    /** write hull transom angle */
    put(e, d.TransomA, "TransomA");

    /** write hull side planks data */
    put(e, d.NBPlank, "NBPlank");
    put(e, d.TopPlankA, "TopPlankA");
    put(e, d.LowPlankA, "lowPlankA");
    // put(e, d.AutoPlank, "AutoPlank");
    //
}


/** Puts a CSailDef sail definition to an XML document.
 *  NEW members shall be added at THE END OF THE LIST in order
 *  to maintain backward comaptibility of files.
 *
 *  See also CSailDoc::get(QDomNode &parent, const CSailDef &d, const QString &name )
 */
void CSailDoc::put(QDomNode &parent, const CSailDef &d, const QString &name )
{
    QDomElement e = createElement("CSailDef",name);
    parent.appendChild(e);

    /** write Sail cut and Sail type */
    put(e, d.sailCut, "sailCut");
    put(e, d.sailType, "sailType");

    /** write Sail Boat data */
    put(e, d.LOA, "LOA");
    put(e, d.foreI, "foreI");
    put(e, d.foreJ, "foreJ");
    put(e, d.tackX, "tackX");
    put(e, d.tackY, "tackY");

    /** write Sides of the sail */
    put(e, d.luffL, "luffL");
    put(e, d.rake, "rake");
    put(e, d.gaffDeg, "gaffDeg");
    put(e, d.gaffL, "gaffL");
    put(e, d.footL, "footL");
    put(e, d.leechL, "leechL");

    /** write Shape of sail sides */
    put(e, d.luffR, "luffR");
    put(e, d.gaffR, "gaffR");
    put(e, d.leechR, "leechR");
    put(e, d.footR, "footR");

    put(e, d.luffRP, "luffRP");
    put(e, d.gaffRP, "gaffRP");
    put(e, d.leechRP, "leechRP");
    put(e, d.footRP, "footRP");

    /** write sail Cloth width, Seam and Hems width */
    put(e, d.clothW, "clothW");
    put(e, d.seamW, "seamW");
    put(e, d.leechHemW, "leechHemW");
    put(e, d.hemsW, "hemsW");

    /** write sail Twist angle*/
    put(e, d.twistDeg, "twistDeg");

    /** write sail Sheeting angle*/
    put(e, d.sheetDeg, "sheetDeg");

    /** write sail Mould */
    put(e, d.mould, "mould");

    //** NOTE: this is the point at which sail data evolutions start */

    /** write sail ID */
    put(e, d.sailID, "sailID");

    /** write sail Radial sections */
    put(e, d.nbSections, "nbSections");

    /** write sail Radial gores */
    put(e, d.nbGores, "nbGores");

    /** write sail Luff gores */
    put(e, d.nbLuffGores, "nbLuffGores");

    /** write sail Wing dihedral angle */
    put(e, d.dihedralDeg, "dihedralDeg");

    /** write sail Foot hem width */
    put(e, d.footHemW, "footHemW");

    /** NOTE: write here below future new elements of sail */
    //
}


/** Puts a CSide sail side to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CSide &s, const QString &name )
{
    QDomElement e = createElement("CSide",name);
    parent.appendChild(e);

    put_vector(e, s, "point");
}


/** Puts a CPanel sail panel to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CPanel &p, const QString &name )
{
    QDomElement e = createElement("CPanel",name);
    parent.appendChild(e);

    put(e, p.left, "left");
    put(e, p.top, "top");
    put(e, p.right, "right");
    put(e, p.bottom, "bottom");

    put(e, p.hasHems, "hasHems");

    if (p.hasHems)
    {
        put(e, p.cutLeft, "cutLeft");
        put(e, p.cutTop, "cutTop");
        put(e, p.cutRight, "cutRight");
        put(e, p.cutBottom, "cutBottom");
    }
}


/** Puts a CPanelGroup to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CPanelGroup &g, const QString &name )
{
    QDomElement e = createElement("CPanelGroup",name);
    parent.appendChild(e);

    put_vector(e, g, "panel");
    put(e, g.title, "title");
    put_vector(e, g.child, "child");
    put(e, g.type, "type");
}


/** Puts a CProfile sail profile to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CProfile &p, const QString &name )
{
    QDomElement e = createElement("CProfile",name);
    parent.appendChild(e);

    put(e, p.getDepth(), "depth");
    put(e, p.getLeech(), "kleech");
    put(e, p.getLuff(), "kluff");
}


/** Puts a CSailMould sail mould to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CSailMould &m, const QString &name )
{
    QDomElement e = createElement("CSailMould",name);
    parent.appendChild(e);

    put(e, m.vertpos, "vertpos");
    put_vector(e, m.profile, "profile");
}


/** Put a CBoatDef boat definition to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CBoatDef &d, const QString &name )
{
    QDomElement e = createElement("CBoatDef",name);
    parent.appendChild(e);

    put_vector(e, d, "element");
}


/** Put a CBoatElement elements of a boat (sail, hull, rig) to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CBoatElement &s, const QString &name )
{
    QDomElement e = createElement("CBoatElement",name);
    parent.appendChild(e);

    put(e, s.filename, "filename");
    put(e, s.type, "type");
    put(e, s.origin, "origin");
    put(e, (CPanelGroup&)s, "panelgroup");
}


/** Puts a CPrefs preferences to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CPrefs& p, const QString& name)
{
    QDomElement e = createElement("CPrefs",name);
    parent.appendChild(e);

    put(e, p.language, "language");
    put_vector(e, p.mruDocuments, "mruDocuments");
    put(e, p.mainWindowHeight, "mainWindowHeight");
    put(e, p.mainWindowWidth, "mainWindowWidth");
}


/** Puts a CRigDef rig definition to an XML document.
 */
void CSailDoc::put(QDomNode &parent, const CRigDef &d, const QString &name )
{
    QDomElement e = createElement("CRigDef",name);
    parent.appendChild(e);

    put(e, d.rigID, "rigID");

    /** write rig fore triangle */
    put(e, d.foreI, "foreI");
    put(e, d.foreJ, "foreJ");
    /** write rig mast data*/
    put(e, d.MHeight, "MHeight");
    put(e, d.MCord, "MCord");
    put(e, d.MWidth, "MWidth");
    put(e, d.MRakeM, "MRakeM");
    put(e, d.MRakeD, "MRakeD");
    put(e, d.MBase, "MBase");
    put(e, d.MRnd, "MRnd");
    put(e, d.MRndPos, "MRndPos");
    /** write rig shrouds data */
    put(e, d.CSH, "CSH");
    put(e, d.CSB, "CSB");
    put(e, d.LSB, "LSB");
    /** write rig spreaders data */
    put(e, d.SPNB, "SPNB");
    put(e, d.SPH[0], "SPH0");
    put(e, d.SPH[1], "SPH1");
    put(e, d.SPH[2], "SPH2");
    put(e, d.SPH[3], "SPH3");
    put(e, d.SPW[0], "SPW0");
    put(e, d.SPW[1], "SPW1");
    put(e, d.SPW[2], "SPW2");
    put(e, d.SPW[3], "SPW3");
    /** write rig mainsail data */
    put(e, d.BAD, "BAD");
    put(e, d.HAD, "HAD");
    //
}


/** Writes the CSailDoc document to file.
 *
 * @throws an exception if writing failed.
 */
void CSailDoc::toFile(const QString &filename)
{
    QFile f( filename );

    if ( !f.open(QIODevice::WriteOnly) )
        throw write_error("CSailDoc::toFile : cannot open file for write access!");

    f.write(toByteArray());
    f.close();
}

