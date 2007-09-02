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

#ifndef GEOCPP_CORE_H
#define GEOCPP_CORE_H

#include <math.h>

// types and constants
typedef double real;

const real EPS = 1E-14; // was 1E-14
const real PI = 3.14159265358979323846;

/** Class for returning information about an exception.
 */
class CException
{
protected:
    /** the exception's error message. */
    const  char *message;
    
public:
    CException (const char *);
    ~CException();
    void report(void);
};

#endif

