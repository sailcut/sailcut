/*
 * Copyright (C) 1993-2006 Robert & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 * 
 * $Id: saildispgl.h,v 1.4 2006/01/25 21:54:02 jeremy_laine Exp $
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

#ifndef SAILDISPGL_H
#define SAILDISPGL_H

#include "saildisp.h"
#include <QGLWidget>

/** An OpenGL display area for a sail. This is only used if Sailcut is
 * compiled with OpenGL support enabled.
 *
 * @see CSailDispLabel
 */
class CSailDispGL : public QGLWidget, public CSailDisp
{
public:
    CSailDispGL( QWidget *parent );

    void redraw();

protected:
    void clear();
    void draw( const CPanel &panel );
    void draw( const CSail &sail );
    void initializeGL();
    void mousePressEvent ( QMouseEvent * );
    void paintGL();
    void resizeGL( int w, int h );
    void wheelEvent( QWheelEvent *event);
};


#endif
