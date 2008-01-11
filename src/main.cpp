/*
 * Copyright (C) 1993-2008 Robert & Jeremy Laine
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

#include "sailcutqt.h"
#include "formmain.h"
#include <geocpp/core.h>

/**
 * Sailcut CAD's main routine.
 */
int main( int argc, char* argv[] )
{
    int retCode = 1;

    // fire up sailcut
    try
    {
        CSailApp app( argc, argv );
        app.readPrefs();

        CFormMain *sailcut = new CFormMain(&app);

        if (argc > 1)
            sailcut->show(argv[1]);
        else
            sailcut->show();

        retCode = app.exec();
        app.writePrefs();
    }
    catch (CException e)
    {
        printf("main() : caught exception, exiting!\n");
    }

    return retCode;
}

