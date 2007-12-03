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

#include "panelgroup.h"

/** Constructs a panel group with the specified number of panels.
 */
CPanelGroup::CPanelGroup( unsigned int nbpanels /* = 0 */)
{
    panel.resize(nbpanels);
    type = SAIL;
}


/** Copy constructor.
 */
CPanelGroup::CPanelGroup( const CPanelGroup& s )
{
    title = s.title;
    panel = s.panel;
    child = s.child;
    type = s.type;
}


/** Construct a panel group from a single panel.
 */
CPanelGroup::CPanelGroup( const CPanel& p )
{
    panel.resize(1);
    panel[0] = p;
}


/** Returns the smallest 3D box that contains all the panels.
 */
CRect3d CPanelGroup::boundingRect() const
{
    CRect3d rect;
    bool rect_filled = 0;
    unsigned int i;

    if ( panel.size() > 0 )
    {
        rect = panel[0].boundingRect();
        for( i = 1; i < panel.size(); i++ )
            rect = rect.join( panel[i].boundingRect() );
        rect_filled = 1;
    }

    if ( child.size() > 0 )
    {
        if (rect_filled)
            rect = rect.join( child[0].boundingRect() );
        else
            rect = child[0].boundingRect();
        
        for( i = 1; i < child.size(); i++ )
            rect = rect.join( child[i].boundingRect() );
    }
    return rect;
}


/** Positions each of the display Sail's panels' label.
     The font size is defined in void CSailDispLabel::drawLabels()
 */
void CPanelGroup::placeLabels()
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
void CPanelGroup::plotLabels()
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
CPanelGroup CPanelGroup::rotate( const CPoint3d &p, const CMatrix &m ) const
{
    unsigned int i;
    CPanelGroup ret = *this;
    for (i = 0; i < panel.size(); i++ )
        ret.panel[i] = panel[i].rotate(p,m);
    for (i = 0; i < child.size(); i++ )
        ret.child[i] = child[i].rotate(p,m);
    return ret;
}


/** Performs an assignment.
 */
CPanelGroup& CPanelGroup::operator=(const CPanelGroup& s)
{
    if (&s == this)
        return *this;

    title = s.title;
    panel = s.panel;
    child = s.child;
    type = s.type;

    return *this;
}


/** Performs a 3D translation of the sail by a given vector.
 */
CPanelGroup CPanelGroup::operator+(const CVector3d& transl) const
{
    unsigned int i;
    CPanelGroup ret = *this;
    for (i = 0; i < panel.size(); i++)
        ret.panel[i] = ret.panel[i] + transl;
    for (i = 0; i < child.size(); i++)
        ret.child[i] = ret.child[i] + transl;
    return ret;
}


/** Outputs a CPanelGroup to a stream.
 */
ostream& operator<<(ostream &o, const CPanelGroup &s)
{
    unsigned int i;
    for(i = 0; i < s.panel.size(); i++)
    {
        o << "===== CPanel : " << i << " ====" << endl;
        o << s.panel[i] << endl;
    }
    for(i = 0; i < s.child.size(); i++)
    {
        o << "===== child CPanelGroup : " << i << " ====" << endl;
        o << s.child[i] << endl;
    }
    return o;
}


