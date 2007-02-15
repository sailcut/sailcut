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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef HAVE_QTOPENGL

#include "saildispgl.h"
#include <QMouseEvent>

/** Construct an OpenGL view area.
 *
 * @param parent the parent widget
 */
CSailDispGL::CSailDispGL( QWidget * parent )
        :  QGLWidget( parent )
{
    if ( !QGLFormat::hasOpenGL() )
        throw CException("This system has no OpenGL support.");
}


/** Clear the display.
 */
void CSailDispGL::clear()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


/** Draw a panel of a sail.
 *
 * @param panel
 */
void CSailDispGL::draw( const CPanel &panel )
{
    CPoint3d pt;
    unsigned int i;

    glBegin(GL_TRIANGLE_STRIP);
    for (i =0; i < panel.top.nbpoints(); i++)
    {
        pt = panel.top.point[i];
        glVertex3d(pt.x(),pt.y(),pt.z());
        pt = panel.bottom.point[i];
        glVertex3d(pt.x(),pt.y(),pt.z());
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    pt = (panel.left.point[0]+panel.left.point[panel.left.nbpoints()-1])*0.5;
    glVertex3d(pt.x(),pt.y(),pt.z());
    for (i =0; i < panel.left.nbpoints(); i++)
    {
        pt = panel.left.point[i];
        glVertex3d(pt.x(),pt.y(),pt.z());
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    pt = (panel.right.point[0]+panel.right.point[panel.right.nbpoints()-1])*0.5;
    glVertex3d(pt.x(),pt.y(),pt.z());
    for (i =0; i < panel.right.nbpoints(); i++)
    {
        pt = panel.right.point[i];
        glVertex3d(pt.x(),pt.y(),pt.z());
    }
    glEnd();
}


/** Draw a complete sail.
 *
 * @param sail
 */
void CSailDispGL::draw( const CPanelGroup &sail )
{
    unsigned int i;
    for( i = 0; i < sail.panel.size(); i++ )
    {
        if ( sail.type == HULL )
        {
            //  grey
            glColor3f(.4, .5, .5);
        }
        else
        {
            // alternate light yellow / yellow panels
            glColor3f(.9, .9, 0.4 + 0.4*(i % 2));
        }

        draw( sail.panel[i] );
    }
    for( i = 0; i < sail.child.size(); i++ )
        draw( sail.child[i] );
}


/** Force a redraw of the view area.
 */
void CSailDispGL::redraw()
{
    updateGL();
}


/** Initialize the OpenGL subsystem.
 */
void CSailDispGL::initializeGL()
{
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
    clear();

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

#endif
