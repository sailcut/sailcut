/*
 * Copyright (C) 2002-2006 Jeremy Laine
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

#include <geocpp/core.h>
#include <iostream>

/** Constructs a CException object.
 */
CException::CException(const string &msg)
{
    message = msg;

    /* FIXME: should we output anything here? */
    cout << "[ exception ] " << message << endl;
}


/** The destructor.
 */
CException::~CException() throw()
{
}


/** Returns a C-style character string describing the general cause of
 *  the current error (the same string passed to the ctor).
 */
const char *CException::what() const throw()
{
    return message.c_str();
}

