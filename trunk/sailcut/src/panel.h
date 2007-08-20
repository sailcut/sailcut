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

#ifndef PANEL_H
#define PANEL_H

#include <vector>
#include <QString>

#include <geocpp/geocpp.h>

enum enumPointType { LUFF, FOOT, LEECH, GAFF };
enum enumAlign {LEFT, LOW_LEFT, BOTTOM};
enum enumDevelopAlign {ALIGN_TOP,ALIGN_BOTTOM };

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
    QString name;

    /** label text height (default : 5) */
    int height;

    /** label text color (default : 1) */
    int color;

    /** origin of the label */
    CPoint3d origin;

    /** direction for writing the label */
    CVector3d direction;

    /** function for rotating a label. */
    CPanelLabel rotate( const CPoint3d &, const CMatrix & ) const;

    /** operator to translate a label */
    CPanelLabel operator+ (const CVector3d &) const;

    /** operator to copy a label */
    CPanelLabel& operator=( const CPanelLabel &);

    friend ostream& operator<< (ostream &, const CPanelLabel &);
};


/*****************************************************************************
 
                               CSide class
 
*****************************************************************************/

/** This class describes a side of a panel
 */
class CSide
{
public:
    CSide( unsigned int = 1 );
    CSide( const CSide& );
    //   ~CSide();

    /** the points that make up the side */
    vector<CPoint3d> point;

    // member functions
public:
    void fill( const CPoint3d &, const CPoint3d & );
    void fill( const CPoint3d &, const CPoint3d &, const CPoint3d & );

    /** Accessor for the number of points. */
    unsigned int nbpoints() const
    {
        return point.size();
    };

    /** function for rotating a side. */
    CSide rotate( const CPoint3d &, const CMatrix &) const;

    // operators
    CSide operator+ (const CVector3d &) const;
    CSide& operator= (const CSide &);
    friend ostream& operator<< (ostream &, const CSide &);
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

/** This class holds a sail panel
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
    CPanel develop(enumDevelopAlign align) const;
    real height() const;    //  height Y of a panel
    
    void placeLabel();   //  place a label at the center of a panel
    CPanel reframe(enumAlign align) const;
    CPanel rotate( const CPoint3d &, const CMatrix &) const;
    CPanel operator+ (const CVector3d &) const;
    CPanel& operator= (const CPanel &);

    friend ostream& operator<< (ostream &, const CPanel &);
};

// global functions
ostream& operator<< (ostream &, const CPanel &);
ostream& operator<< (ostream &, const CSide &);
ostream& operator<< (ostream &, const CPanelLabel &);

#endif
