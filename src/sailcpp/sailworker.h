/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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

#ifndef SAILWORKER_H
#define SAILWORKER_H

#include "panelgroup.h"
#include "saildef.h"

// forward declarations
class CPanelGroup;
class CSeam;

enum enumEdgeType { LUFF_EDGE, GAFF_EDGE, FOOT_EDGE, LEECH_EDGE };

/** The CSailWorker class does all the sail-related calculations like laying the panels
 *  It is used to create the sail from a CSailDef definition.
 *
 * @ingroup SailCpp
 * @see CSailDef, CPanelGroup
 */
class CSailWorker : public CSailDef
{
public:
    CSailWorker(const CSailDef &s);

    real Area();
    real Diagonal();
    real IRCwidth( const real &h );
    real SailWidth( const real &h );
    real LeechLength( const real &h );
    real LuffLength( const real &h );
    real SailLP( );

    CPanelGroup makeSail() const;
    CPanelGroup makeSail(CPanelGroup &flatsail, CPanelGroup &dispsail) const;

    /** position of tack point */
    CPoint3d tack;
    /** position of head point */
    CPoint3d head;
    /** position of peak point */
    CPoint3d peak;
    /** position of clew point */
    CPoint3d clew;

protected:
    /** type of Edge of sail */
    enumEdgeType Edge;
    
    /** layout of sail's panels */
    CPanelGroup Layout0( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; // CROSS
    CPanelGroup LayoutTwist( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; // TWIST
    CPanelGroup LayoutVertical( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; // VERTICAL
    CPanelGroup LayoutRadial( CPanelGroup &flatsail, CPanelGroup &dispsail ) const;// RADIAL
    CPanelGroup LayoutMitre( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; // MITRE
    CPanelGroup LayoutTriRadial( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; // TRIRADIAL
    CPanelGroup LayoutWing( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; // WING

    /** intersections with edges */
    CPoint3d EdgeIntersect( const enumEdgeType &Edge, const CPoint3d &pt1, const CVector3d &v1 ) const;
    CPoint3d FwdIntersect( const CPoint3d &pt1 ) const;
    CPoint3d AftIntersect( const CPoint3d &pt1 ) const;
    CPoint3d MitreIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    
    /** mitre intersection point with luff */
    CPoint3d mitreLuffPt;
    
    /** depth at any point */
    CPoint3d Zpoint( const CPoint3d &p1 ) const;
    /** depth for all points */
    CPanel Zpanel( const CPanel &p1 ) const;

    /** foot vector */
    CVector3d footV;
    /** unitary vector perpendicular to foot */
    CVector3d footVP;
    /** gaff vector */
    CVector3d gaffV;
    /** unitary vector perpendicular to gaff */
    CVector3d gaffVP;
    /** leech vector */
    CVector3d leechV;
    /** unitary vector perpendicular to leech */
    CVector3d leechVP;
    /** luff vector */
    CVector3d luffV;
    /** unitary vector perpendicular to luff */
    CVector3d luffVP;
    /** mitre vector */
    CVector3d mitreV;
    
    /** 3D foot straight line */
    CSubSpace footLine;
    /** 3D gaff straight line */
    CSubSpace gaffLine;
    /** 3D leech straight line */
    CSubSpace leechLine;
    /** 3D luff straight line */
    CSubSpace luffLine;
    /** 3D mitre straight line */
    CSubSpace mitreLine;
};

#endif
