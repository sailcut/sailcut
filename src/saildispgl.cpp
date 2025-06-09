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

#include <cmath>

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>

#include "saildispgl.h"

static const char* vertexShader =
    "attribute mediump vec4 posAttr;\n"
    "uniform mediump mat4 matrixAttr;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = matrixAttr * posAttr;\n"
    "}\n";

static const char* fragmentShader =
    "uniform mediump vec4 colorAttr;\n"
    "void main()\n"
    "{\n"
    "     gl_FragColor = colorAttr;\n"
    "}\n";


/** Construct an OpenGL view area.
 *
 * @param parent the parent widget
 */
CSailDispGL::CSailDispGL(QWidget * parent)
    : QOpenGLWidget(parent)
{
}

void CSailDispGL::putPoint(GLfloat **vertex, const CPoint3d &pt) const
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
    const int mainCount = panel.top.size() * 2;
    const int leftCount = panel.left.size() + 1;
    const int rightCount = panel.right.size() + 1;
    const int maxCount = qMax(mainCount, qMax(leftCount, rightCount));
    const int vertexSize = 3;
    GLfloat vertexArray[maxCount * vertexSize];
    GLfloat *vertex;

    // main
    vertex = vertexArray;
    for (i = 0; i < panel.top.size(); i++) {
        putPoint(&vertex, panel.top[i]);
        putPoint(&vertex, panel.bottom[i]);
    }
    program->enableAttributeArray(posAttr);
    program->setAttributeArray(posAttr, vertexArray, vertexSize);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, mainCount);
    program->disableAttributeArray(posAttr);

    // left side
    vertex = vertexArray;
    putPoint(&vertex, (panel.left.front() + panel.left.back()) * 0.5);
    for (i = 0; i < panel.left.size(); i++)
        putPoint(&vertex, panel.left[i]);
    program->enableAttributeArray(posAttr);
    program->setAttributeArray(posAttr, vertexArray, vertexSize);
    glDrawArrays(GL_TRIANGLE_FAN, 0, leftCount);
    program->disableAttributeArray(posAttr);

    // right side
    vertex = vertexArray;
    putPoint(&vertex, (panel.right.front() + panel.right.back()) * 0.5);
    for (i =0; i < panel.right.size(); i++)
        putPoint(&vertex, panel.right[i]);
    program->enableAttributeArray(posAttr);
    program->setAttributeArray(posAttr, vertexArray, vertexSize);
    glDrawArrays(GL_TRIANGLE_FAN, 0, rightCount);
    program->disableAttributeArray(posAttr);
}


/** Draw a complete sail.
 *
 * @param sail
 */
void CSailDispGL::draw( const CPanelGroup &sail )
{
    unsigned int i;
    for (i = 0; i < sail.size(); i++) {
        if (sail.type == HULL) {
            // Hull color (green)
            program->setUniformValue(colorAttr, QColor(26, 128, 51));
        } else if ( sail.type == RIG) {
            // Rig color (dark red)
            program->setUniformValue(colorAttr, QColor(128, 26, 26));
        } else {
            // Sail color (alternate dark yellow / yellow / white)
            program->setUniformValue(colorAttr, QColor(204, 179 + 12 * (i % 3), 102 + 51 * (i % 3)));
        }
        draw(sail[i]);
    }
    for (i = 0; i < sail.child.size(); i++)
        draw(sail.child[i]);
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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepthf(1.0f);

    program = new QOpenGLShaderProgram(this);
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        qWarning("could not load vertex shader");
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        qWarning("could not load fragment shader");
    program->link();

    colorAttr = program->uniformLocation("colorAttr");
    matrixAttr = program->uniformLocation("matrixAttr");
    posAttr = program->attributeLocation("posAttr");
}


/** We received a mouse click.
 *
 * @param event
 */
void CSailDispGL::mousePressEvent ( QMouseEvent *event )
{
    if (event->button() == Qt::LeftButton)
    {
        setCenter(screenToLogical(event->pos().x(), event->pos().y()));
        redraw();
    }
}


/** Perform OpenGL painting.
 */
void CSailDispGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set transform
    const CRect3d lRect = logicalRect();
    QMatrix4x4 matrix;
    matrix.scale(
        real(2) / lRect.width(),
        real(2) / lRect.height(),
        - real(2) / sqrt(lRect.width() * lRect.width() + lRect.height() * lRect.height()));
    matrix.translate(-center().x(), -center().y(), -center().z());

    program->bind();
    program->setUniformValue(matrixAttr, matrix);
    draw(dispObject());
    program->release();
}


/** The draw area has been resized.
 *
 * @param w the new width
 * @param h the new height
 */
void CSailDispGL::resizeGL( int w, int h )
{
    glViewport(0, 0, (GLint)w, (GLint)h);
    setViewRect(CRect3d(CPoint3d(0, 0, 0), CPoint3d(w, h, 0)));
}


/** We received a mouse wheel movement.
 *
 * @param event
 */
void CSailDispGL::wheelEvent( QWheelEvent *event)
{
    const int delta = event->pixelDelta().y();
    if (delta > 0)
        zoomIn();
    else if (delta < 0)
        zoomOut();
    redraw();
}
