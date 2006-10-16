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

#ifndef SAIL_H
#define SAIL_H

#include "panel.h"


/** This class holds a 3D or developed sail.
 */
class CSail
{
public:
    CSail( unsigned int = 0 );
    CSail( const CSail& );
    CSail( const CPanel& );

    /** the sail's panels */
    vector<CPanel> panel;

    /** title of the sail */
    QString sailID;

    // member functions
public:
    /** rectangle containing a panel. */
    CRect3d boundingRect() const;

    /** accessor for the number of panels. */
    unsigned int nbpanels() const
    {
        return panel.size();
    };

    /** function to place the labels on a displayed sail */
    void placeLabels();
    /** function to place the labels on a plotted sail */
    void plotLabels();

    /** function for rotating a panel. */
    CSail rotate( const CPoint3d &, const CMatrix & ) const;

    // operators
    CSail& operator=( const CSail &);
    CSail operator+ (const CVector3d &) const;
    friend ostream& operator<< (ostream &, const CSail &);
};


// global functions
ostream& operator<< (ostream &, const CSail &);

#endif
