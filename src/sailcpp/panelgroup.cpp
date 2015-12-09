/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
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

#include <cstdio>

#include "panelgroup.h"

/** Constructs a panel group with the specified number of panels.
 */
CPanelGroup::CPanelGroup( unsigned int nbpanels /* = 0 */)
{
    resize(nbpanels);
    type = SAIL;
}


/** Copy constructor.
 */
CPanelGroup::CPanelGroup( const CPanelGroup& s )
        : vector<CPanel>(s)
{
    title = s.title;
    child = s.child;
    type = s.type;
}


/** Construct a panel group from a single panel.
 */
CPanelGroup::CPanelGroup( const CPanel& p )
{
    resize(1);
    at(0) = p;
}


/** Returns the smallest 3D box that contains all the panels.
 */
CRect3d CPanelGroup::boundingRect() const
{
    CRect3d rect;
    bool rect_filled = 0;
    unsigned int i;

    if ( size() > 0 )
    {
        rect = at(0).boundingRect();
        for (i = 1; i < size(); i++)
            rect = rect.join( at(i).boundingRect() );
        rect_filled = 1;
    }

    if ( child.size() > 0 )
    {
        if (rect_filled)
            rect = rect.join( child[0].boundingRect() );
        else
            rect = child[0].boundingRect();

        for (i = 1; i < child.size(); i++)
            rect = rect.join( child[i].boundingRect() );
    }
    return rect;
}


/** Positions each of the display Sail's panels' label.
     The font size is defined in void CSailDispLabel::drawLabels()
 */
void CPanelGroup::placeLabels()
{
    for (unsigned int i = 0; i < size(); i++)
    {
        char buffer[32];
        sprintf(buffer, "%i", i);
        at(i).label.name = buffer;
        at(i).placeLabel();
    }
}


/** Positions each of the plotted Sail's panels' label.
     The font size is default 5 mm
 */
void CPanelGroup::plotLabels()
{
    for (unsigned int i = 0; i < size(); i++)
    {
        char buffer[32];
        sprintf(buffer, "%i", i);
        at(i).label.name = buffer;

        at(i).label.height = 5;
        // position the label and orientate it
        at(i).label.origin = at(i).bottom[2];
        at(i).label.direction = CVector3d( at(i).bottom[3]- at(i).bottom[2]);
    }
}


/** Transform a sail.
 */
CPanelGroup CPanelGroup::transformed(const QMatrix4x4 &m) const
{
    unsigned int i;
    CPanelGroup ret = *this;
    for (i = 0; i < size(); i++)
        ret[i] = at(i).transformed(m);
    for (i = 0; i < child.size(); i++)
        ret.child[i] = child[i].transformed(m);
    return ret;
}


/** Performs an assignment.
 */
CPanelGroup& CPanelGroup::operator=(const CPanelGroup& s)
{
    if (&s == this)
        return *this;

    this->vector<CPanel>::operator=(s);
    title = s.title;
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
    for (i = 0; i < size(); i++)
        ret[i] = ret[i] + transl;
    for (i = 0; i < child.size(); i++)
        ret.child[i] = ret.child[i] + transl;
    return ret;
}


/** Outputs a CPanelGroup to a stream.
 */
ostream& operator<<(ostream &o, const CPanelGroup &s)
{
    unsigned int i;
    for (i = 0; i < s.size(); i++)
    {
        o << "===== CPanel : " << i << " ====" << endl;
        o << s[i] << endl;
    }
    for (i = 0; i < s.child.size(); i++)
    {
        o << "===== child CPanelGroup : " << i << " ====" << endl;
        o << s.child[i] << endl;
    }
    return o;
}


