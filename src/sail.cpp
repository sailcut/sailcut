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

#include "sail.h"

/*****************************************************************************
 
                              CSail class
 
*****************************************************************************/

/** Constructs a sail with the specified number of panels.
 */
CSail::CSail( unsigned int nbpanels /* = 0 */)
{
    panel.resize(nbpanels);
}


/** Copy constructor
 */
CSail::CSail( const CSail& s )
{
    panel = s.panel;
    sailID = s.sailID;
}


/** Construct a sail from a single panel
 */
CSail::CSail( const CPanel& p )
{
    panel.resize(1);
    panel[0] = p;
}


/** Returns the smallest 3D box that contains the sail.
 */
CRect3d CSail::boundingRect() const
{
    CRect3d rect;

    if ( panel.size() == 0 )
        return rect;

    rect = panel[0].boundingRect();
    for( unsigned int i = 1; i < panel.size(); i++)
    {
        rect = rect.join(panel[i].boundingRect());
    }
    return rect;
}


/** Positions each of the display Sail's panels' label.
     The font size is defined in void CSailDispLabel::drawLabels()
 */
void CSail::placeLabels()
{
    for (unsigned int i = 0; i < panel.size(); i++)
    {
        char buffer[32];
        sprintf(buffer, "%i", i);
        panel[i].label.name = buffer;
        panel[i].placeLabel();
    }
}


/** Positions each of the plotted Sail's panels' label.
     The font size is default 5 mm
 */
void CSail::plotLabels()
{
    for (unsigned int i = 0; i < panel.size(); i++)
    {
        char buffer[32];
        sprintf(buffer, "%i", i);
        panel[i].label.name = buffer;

        panel[i].label.height = 5;
        // position the label and orientate it
        panel[i].label.origin = panel[i].bottom.point[2];
        panel[i].label.direction = CVector3d( panel[i].bottom.point[3]- panel[i].bottom.point[2]);
    }
}


/** Rotates a sail around a point.
 */
CSail CSail::rotate( const CPoint3d &p, const CMatrix &m ) const
{
    CSail ret = *this;
    for (unsigned int i = 0; i < panel.size(); i++ )
    {
        ret.panel[i] = panel[i].rotate(p,m);
    }
    return ret;
}


/** Performs an assignment.
 */
CSail& CSail::operator=(const CSail& s)
{
    if (&s == this)
        return *this;

    panel = s.panel;
    sailID = s.sailID;

    return *this;
}


/** Performs a 3D translation of the sail by a given vector.
 */
CSail CSail::operator+(const CVector3d& transl) const
{
    CSail ret = *this;
    for (unsigned int i = 0; i < panel.size(); i++)
    {
        ret.panel[i] = ret.panel[i] + transl;
    }
    return ret;
}


/*********************************************
 
            Global functions
 
 *********************************************/

/** Outputs a CSail  to a stream.
 */
ostream& operator<<(ostream &o, const CSail &s)
{
    for(unsigned int i=0; i<s.nbpanels(); i++)
    {
        o << "===== CPanel : " << i << " ====" << endl;
        o << s.panel[i] << endl;
    }
    return o;
}