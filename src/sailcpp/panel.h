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

#ifndef PANEL_H
#define PANEL_H

#include <Qt>

#include <vector>
#include <string>

#include <geocpp/geocpp.h>

enum enumPointType { LUFF, FOOT, LEECH, GAFF };
enum enumAlign {LEFT, LOW_LEFT, BOTTOM};
enum enumDevelopAlign {ALIGN_TOP,ALIGN_BOTTOM };

class panel_error : public std::runtime_error
{
public:
    panel_error(const std::string &message) : std::runtime_error(message)
    {
        std::cout << "in panel: " << what() << std::endl;
    }
};

/*****************************************************************************

                                CPanelLabel class

*****************************************************************************/
/** This class describe a label for a panel
*/
class CPanelLabel
{
public:
    CPanelLabel();
    CPanelLabel( const CPanelLabel& );

    /** label name of the panel */
    std::string name;

    /** label text height (default : 5) */
    int height;

    /** label text color (default : 1) */
    int color;

    /** origin of the label */
    CPoint3d origin;

    /** direction for writing the label */
    CVector3d direction;

    CPanelLabel transformed(const CMatrix4x4 &m) const;

    /** operator to copy a label */
    CPanelLabel& operator=( const CPanelLabel &);

    friend std::ostream& operator<< (std::ostream &, const CPanelLabel &);
};


/*****************************************************************************

                               CSide class

*****************************************************************************/

/** The CSide class describes a side of a panel. It is a collection of points.
 *
 * @ingroup SailCpp
 */
class CSide : public std::vector<CPoint3d>
{
public:
    CSide( unsigned int = 1 );

public:
    void fill( const CPoint3d &, const CPoint3d & );
    void fill( const CPoint3d &, const CPoint3d &, const CPoint3d & );

    CSide transformed(const CMatrix4x4 &m) const;

    // operators
    friend std::ostream& operator<< (std::ostream &, const CSide &);
};


/** This class describes a seam of a sail.
 *
 * @see CSide
 */
class CSeam : public CSide
{
public:
    /** Constructor */
    CSeam( unsigned int i = 1 ) : CSide( i )
    {}
    ;
    /** type of the left point */
    enumPointType leftPoint;
    /** type of the right point */
    enumPointType rightPoint;
};


/*****************************************************************************

                          Panel class

*****************************************************************************/

/** The CPanel describes a sail panel.
 *
 *  A panel has at least four sides: top, bottom, left and right.
 *  A developed panel also has four additional sides representing
 *  the edge of the cloth taking into account seam width and hems width.
 *
 * @ingroup SailCpp
 */
class CPanel
{
public:
    CPanel();
    CPanel( const CPanel &p );

    /** panel's label */
    CPanelLabel label;

    /** panel's left side */
    CSide left;
    /** panel's right side */
    CSide right;
    /** panel's top side */
    CSide top;
    /** panel's bottom side */
    CSide bottom;

    /** panel's cut left side */
    CSide cutLeft;
    /** panel's cut right side */
    CSide cutRight;
    /** panel's cut top side */
    CSide cutTop;
    /** panel's cut bottom side */
    CSide cutBottom;

    /** do we have hems added to the panel edges? */
    bool hasHems;

    // member functions
    CRect3d boundingRect() const;
    CPoint3d centroid() const; // centroid of a panel
    void addHems( const real &, const real &, const real &, const real &);
    void add6Hems( const real &, const real &, const real &, const real &, const real &, const real & );

    CPanel develop(enumDevelopAlign align) const;

    void placeLabel();   //  place a label at the center of a panel
    CPanel reframe(enumAlign align) const;
    CPanel rotated(const CPoint3d &, real angle, Qt::Axis axis) const;
    CPanel transformed(const CMatrix4x4 &m) const;
    CPanel operator+ (const CVector3d &) const;
    CPanel& operator= (const CPanel &);

    friend std::ostream& operator<< (std::ostream &, const CPanel &);
};

// global functions
std::ostream& operator<< (std::ostream &, const CPanel &);
std::ostream& operator<< (std::ostream &, const CSide &);
std::ostream& operator<< (std::ostream &, const CPanelLabel &);

CVector3d rotateNormalized(real angle, const CVector3d &v);

#endif
