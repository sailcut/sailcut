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

#include "prefs.h"


/**
 * Removes an entry from the Most Recently Used files.
 *
 * @param oldEntry The entry to be removed
 */
void CPrefsMru::removeEntry( const QString& oldEntry )
{
    for (unsigned int i = 0; i < size(); i++)
        if ( at(i) == oldEntry )
            erase(begin()+i);
}


/**
 * Puts an entry at the top of the Most Recently Used files.
 *
 * @param newEntry The entry to be added
 */
void CPrefsMru::touchEntry( const QString &newEntry )
{
    removeEntry(newEntry);
    insert(begin(),newEntry);

    // limit the number of entries
    if (size() > 9)
        resize(9);
}


/**
 * Constructs a class holding the user's preferences.
 */
CPrefs::CPrefs() :
        helpWindowWidth(800), helpWindowHeight(600),
        mainWindowWidth(800), mainWindowHeight(600)
{}

