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

#include <cmath>

#include <QMouseEvent>

#include "saildispgl.h"


/** Construct an OpenGL view area.
 *
 * @param parent the parent widget
 */
CSailDispGL::CSailDispGL(QWidget * parent)
    : QOpenGLWidget(parent)
    , wasResized(true)
{
}

static void putPoint(GLfloat **vertex, const CPoint3d &pt)
{
    *((*vertex)++) = pt.x();
    *((*vertex)++) = pt.y();
    *((*vertex)++) = pt.z();
}

/** Draw a panel of a sail.
 *
 * @param panel
 */
void CSailDispGL::draw( const CPanel &panel )
{
    unsigned int i;
    GLfloat *vertexArray;
    GLfloat *vertex;
    int vertexCount;
    const int vertexSize = 3;

    // main
    vertexCount = panel.top.size() * 2;
    vertexArray = new GLfloat[vertexCount * vertexSize];
    vertex = vertexArray;
    for (i = 0; i < panel.top.size(); i++) {
        putPoint(&vertex, panel.top[i]);
        putPoint(&vertex, panel.bottom[i]);
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(vertexSize, GL_FLOAT, 0, vertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    delete[] vertexArray;

    // left side
    vertexCount = panel.left.size() + 1;
    vertexArray = new GLfloat[vertexCount * vertexSize];
    vertex = vertexArray;
    putPoint(&vertex, (panel.left[0]+panel.left[panel.left.size()-1])*0.5);
    for (i = 0; i < panel.left.size(); i++)
        putPoint(&vertex, panel.left[i]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(vertexSize, GL_FLOAT, 0, vertexArray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    delete[] vertexArray;

    // right side
    vertexCount = panel.right.size() + 1;
    vertexArray = new GLfloat[vertexCount * vertexSize];
    vertex = vertexArray;
    putPoint(&vertex, (panel.right[0]+panel.right[panel.right.size()-1])*0.5);
    for (i =0; i < panel.right.size(); i++)
        putPoint(&vertex, panel.right[i]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(vertexSize, GL_FLOAT, 0, vertexArray);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    delete[] vertexArray;
}


/** Draw a complete sail.
 *
 * @param sail
 */
void CSailDispGL::draw( const CPanelGroup &sail )
{
    unsigned int i;
    for (i = 0; i < sail.size(); i++)
    {
        if ( sail.type == HULL )
        {
            /**  Hull GL color dark green (.1, .3, .2)
                 color lighter green  (.1, .5, .2) */
            glColor3f(.1, .5, .2);
        }
        else if ( sail.type == RIG )
        {
            /**  Rig GL color dark red (.5, .1, .1) */
            glColor3f(.5, .1, .1);
        }
        else
        {
            /** Sail GL color alternate dark yellow / yellow panels / white */
            glColor3f( .8, .7 + 0.05*(i % 3), 0.4 + 0.2*(i % 3));
        }
        draw( sail[i] );
    }
    for (i = 0; i < sail.child.size(); i++)
        draw( sail.child[i] );
}


/** Force a redraw of the view area.
 */
void CSailDispGL::redraw()
{
    update();
}


/** Initialize the OpenGL subsystem.
 */
void CSailDispGL::initializeGL()
{
    initializeOpenGLFunctions();

    // Set up the rendering context, define display lists etc.:
    glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glClearDepth(1.0f);
    // lighting
    GLfloat LightAmbient[]= { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat LightDiffuse[]= { 0.00015f, 0.00015f, 0.00015f, 1.0f };
    GLfloat LightPosition[]= { 1.0f, 1.0f, 1.0f, 1.0f };

    // Setup the ambient & diffuse lamps
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);

    // Enable lighting
    glEnable(GL_LIGHT1);
}


/** We received a mouse click.
 *
 * @param event
 */
void CSailDispGL::mousePressEvent ( QMouseEvent *event )
{
    if (event->button() == Qt::LeftButton)
    {
        setCenter(screenToLogical(event->pos().x(),event->pos().y()));
        redraw();
    }
}


/** Perform OpenGL painting.
 */
void CSailDispGL::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( wasResized )
    {
        glViewport( 0, 0, (GLint)resizeW, (GLint)resizeH );
        setViewRect( CRect3d(CPoint3d(0,0,0), CPoint3d(resizeW,resizeH,0)) );
        wasResized = 0;
    }

    // set coordinate system to match the logical viewport
    glLoadIdentity();
    CRect3d lRect = getLogicalRect();
    real zs = real(2) / sqrt(lRect.width()*lRect.width() + lRect.height()*lRect.height());
    glScaled(real(2) / lRect.width(),real(2) / lRect.height(),zs);
    glTranslated(-center.x(),-center.y(),-center.z());

    draw( dispObject );
}


/** The draw area has been resized.
 *
 * @param w the new width
 * @param h the new height
 */
void CSailDispGL::resizeGL( int w, int h )
{
    resizeW = w;
    resizeH = h;
    wasResized = 1;
}


/** We received a mouse wheel movement.
 *
 * @param event
 */
void CSailDispGL::wheelEvent( QWheelEvent *event)
{
    if (event->delta()>0)
        zoomIn();
    else
        zoomOut();
    redraw();
}
