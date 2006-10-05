/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 *
 * $Id: sailworker.h,v 1.13 2006/10/03 13:05:24 jeremy_laine Exp $
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

#include "sail.h"
#include "saildef.h"

#include "geocpp/geometry.h"
#include "sailcalc.h"

// forward declarations
class CSail;
class CSeam;


/** The CSailWorker class does all the sail-related calculations. It is used to create
 * the sail from its definition.
 *
 * @see CSailDef, CSail
 */
class CSailWorker : public CSailDef
{
public:
    CSailWorker(const CSailDef &s);

    real Area();
    real Diagonal();
    real IRCwidth( const real &h );
    real SailW( const real &h );
    real LeechLength( const real &h );
    real LuffLength( const real &h );
    real SailLP( );

    // create a panel representing the deck
    CPanel makeDeck() const;
    CSail makeSail() const;
    CSail makeSail(CSail &flatsail, CSail &dispsail) const;

    /** position of tack point */
    CPoint3d tack;
    /** position of head point */
    CPoint3d head;
    /** position of peak point */
    CPoint3d peak;
    /** position of clew point */
    CPoint3d clew;

protected:
    /** layout of sail's panels */
    CSail Layout0( CSail &flatsail, CSail &dispsail ) const; // CROSS
    CSail LayoutTwist( CSail &flatsail, CSail &dispsail ) const; // TWIST
    CSail LayoutVertical( CSail &flatsail, CSail &dispsail ) const; // VERTICAL
    CSail LayoutRadial( CSail &flatsail, CSail &dispsail ) const;// RADIAL
    CSail LayoutMitre( CSail &flatsail, CSail &dispsail ) const; // MITRE
    CSail LayoutTriRadial( CSail &flatsail, CSail &dispsail ) const; // TRIRADIAL
    CSail LayoutWing( CSail &flatsail, CSail &dispsail ) const; // WING

    /** intersections with edges */
    CPoint3d FwdIntersect( const CPoint3d &p1 ) const;
    CPoint3d AftIntersect( const CPoint3d &p1 ) const;
    CPoint3d FootIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    CPoint3d GaffIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    CPoint3d LeechIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    CPoint3d LuffIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    CPoint3d MitreIntersect( const CPoint3d &pt1, const CVector3d &v1 ) const;
    /** mitre intersection point with luff */
    CPoint3d mitrePt;
    
    /** depth at any point */
    CPoint3d Zpoint( const CPoint3d &p1 ) const;

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
    CSubSpace foot;
    /** 3D gaff straight line */
    CSubSpace gaff;
    /** 3D leech straight line */
    CSubSpace leech;
    /** 3D luff straight line */
    CSubSpace luff;
    /** 3D mitre straight line */
    CSubSpace mitre;
};

#endif
