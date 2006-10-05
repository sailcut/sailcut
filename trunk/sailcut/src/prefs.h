/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: prefs.h,v 1.10 2006/01/28 16:04:02 jeremy_laine Exp $
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

#ifndef PREFS_H
#define PREFS_H

#include <vector>
#include <QString>

using namespace std;

/**
 * A list of most recently used (MRU) files.
 */
class CPrefsMru : public vector<QString>
{
public:
    /** Construct a new list of Most Recently Used (MRU) files. */
    CPrefsMru()
    {}
    ;
    void touchEntry(const QString& newentry);
    void removeEntry(const QString& oldentry);
};


/**
 * A class used to store a user's preferences.
 */
class CPrefs
{
public:
    CPrefs();

    /** The most recently used rig definitions. */
    CPrefsMru mruRigdef;
    /** The most recently used sail definitions. */
    CPrefsMru mruSaildef;

    /** The width of the help window */
    int helpWindowWidth;
    /** The height of the help window */
    int helpWindowHeight;

    /** The width of the rig window */
    int rigWindowWidth;
    /** The height of the rig window */
    int rigWindowHeight;

    /** The width of the sail window */
    int sailWindowWidth;
    /** The height of the sail window */
    int sailWindowHeight;
    /** The preferred language. */
    QString language;
};

#endif

