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

#ifndef PREFS_H
#define PREFS_H

#include <vector>
#include <QString>

using namespace std;

/**
 * A class used to store a user's preferences.
 */
class CPrefs
{
public:
    CPrefs();

    /** The most recently used documents. */
    vector<QString> mruDocuments;

    /** The width of the help window */
    int helpWindowWidth;
    /** The height of the help window */
    int helpWindowHeight;

    /** The width of the main window */
    int mainWindowWidth;
    /** The height of the main window */
    int mainWindowHeight;

    /** The preferred language. */
    QString language;
};

#endif

