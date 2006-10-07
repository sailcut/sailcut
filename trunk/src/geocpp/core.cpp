/*
 * Copyright (C) 2002-2004 Jeremy Lain�
 * See AUTHORS file for a full list of contributors.
 * 
 * $Id: core.cpp,v 1.13 2004/10/26 19:30:52 jeremy_laine Exp $
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

#include "core.h"
#include <iostream>

using namespace std;

/** Constructs a CException object.
 */
CException::CException (const char * msg)
{
    message = msg;
    report();
}


/** The destructor.
 */
CException::~CException ()
{}


/** Outputs the description of an exception to the standard output.
 */
void CException::report(void)
{
    cout << "[ exception ] " << message << endl;
}
