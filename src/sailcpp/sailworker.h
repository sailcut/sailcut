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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef SAILWORKER_H
#define SAILWORKER_H

#include "panelgroup.h"
#include "saildef.h"

// forward declarations
class CPanelGroup;
class CSeam;

enum enumEdgeType { LUFF_EDGE, GAFF_EDGE, FOOT_EDGE, LEECH_EDGE };

/** The CSailWorker class does all the sail-related calculations like laying the panels.
 *  It is used to create the sail from a CSailDef definition.
 *
 * @ingroup SailCpp
 * @see CSailDef, CPanelGroup
 */
class CSailWorker : public CSailDef
{
public:
    CSailWorker(const CSailDef &s);

    /** The area of the sail. */
    real Area();
    /** The head to clew diagonal length. */
    real Diagonal();
    /** The computation of the width as per IRC rule. */
    real IRCwidth( const real &h );
    /** The computation of the sail width at any height. */
    real SailWidth( const real &h );
    /** The length of the leech measured along its curved edge. */
    real LeechLength( const real &h );
    /** The length of the luff measured along its curved edge. */
    real LuffLength( const real &h );
    /** The maximum width of the sail perpendicular to the luff. */ 
    real SailLP( );

    CPanelGroup makeSail() const;
    CPanelGroup makeSail(CPanelGroup &flatsail, CPanelGroup &dispsail) const;

    /** The Tack 3D point. */
    CPoint3d tack;
    /** The Head 3D point. */
    CPoint3d head;
    /** The Peak 3D point. */
    CPoint3d peak;
    /** The Clew 3D point. */
    CPoint3d clew;

protected:
    /** The Cross cut and Horizontal cut layout of sail's panels. */
    CPanelGroup Layout0( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 
    /** The Twist cut layout of sail's panels. */
    CPanelGroup LayoutTwist( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 
    /** The Vertical cut layout of sail's panels. */
    CPanelGroup LayoutVertical( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 
    /** The Radial cut layout of sail's panels. */
    CPanelGroup LayoutRadial( CPanelGroup &flatsail, CPanelGroup &dispsail ) const;
    /** The Mitre cut layout of sail's panels perpendicular to leech and foot. */
    CPanelGroup LayoutMitre( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 
    /** The Mitre 2 cut layout of sail's panels parralel to foot and leech. */
    CPanelGroup LayoutMitre2( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 
    /** The Tri Radial cut layout of sail's panels. */
    CPanelGroup LayoutTriRadial( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 
    /** The Wing cut layout of sail's panels. */
    CPanelGroup LayoutWing( CPanelGroup &flatsail, CPanelGroup &dispsail ) const; 

    /** The type of sail Edge for an intersection. */
    enumEdgeType Edge;
     /** The intersection of a line defined by a point and a vector with an edge. */
    CPoint3d EdgeIntersect( const enumEdgeType &Edge, const CPoint3d &pt1, const CVector3d &v1 ) const;
    /** The horizontal intersection with forward edge. */
    CPoint3d FwdIntersect( const CPoint3d &pt1 ) const;
    /** The horizontal intersection with aft edge. */
    CPoint3d AftIntersect( const CPoint3d &pt1 ) const;
    /** The intersection of a line defined by a point and a vector with the mitre line. */
    CPoint3d MitreIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    
    /** The mitre intersection point with luff. */
    CPoint3d mitreLuffPt;
    
    /** The depth of the sail at a point. */
    CPoint3d Zpoint( const CPoint3d &p1 ) const;
    /** The depth of the sail at all points of a panel. */
    CPanel Zpanel( const CPanel &p1 ) const;

    /** Foot vector. */
    CVector3d footV;
    /** Unitary vector perpendicular to foot. */
    CVector3d footVP;
    /** Gaff vector. */
    CVector3d gaffV;
    /** Unitary vector perpendicular to gaff. */
    CVector3d gaffVP;
    /** Leech vector. */
    CVector3d leechV;
    /** Unitary vector perpendicular to leech. */
    CVector3d leechVP;
    /** Luff vector. */
    CVector3d luffV;
    /** Unitary vector perpendicular to luff. */
    CVector3d luffVP;
    /** Mitre vector. */
    CVector3d mitreV;
    
    /** The foot straight 3D line. */
    CSubSpace footLine;
    /** The gaff straight 3D line. */
    CSubSpace gaffLine;
    /** The leech straight 3D line. */
    CSubSpace leechLine;
    /** The luff straight 3D line. */
    CSubSpace luffLine;
    /** The mitre straight 3D line. */
    CSubSpace mitreLine;
};

#endif
