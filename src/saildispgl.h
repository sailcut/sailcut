/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
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

#ifndef SAILDISPGL_H
#define SAILDISPGL_H

#include "saildisp.h"

#include <QOpenGLFunctions>
#include <QOpenGLWidget>


class QOpenGLShaderProgram;


/** An OpenGL display area for a sail. This is only used if Sailcut is
 * compiled with OpenGL support enabled.
 *
 * @see CSailDispLabel
 */
class CSailDispGL : public QOpenGLWidget, public QOpenGLFunctions, public CSailDisp
{
public:
    CSailDispGL( QWidget *parent );

    void redraw();

protected:
    void draw( const CPanel &panel );
    void draw( const CPanelGroup &sail );
    void initializeGL();
    void mousePressEvent ( QMouseEvent * );
    void paintGL();
    void resizeGL( int w, int h );
    void wheelEvent( QWheelEvent *event);

private:
    void putPoint(GLfloat **vertex, const CPoint3d &pt) const;

    QOpenGLShaderProgram *program;
    int colorAttr;
    int matrixAttr;
    int posAttr;
};


#endif
