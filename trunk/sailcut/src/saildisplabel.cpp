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

#include "saildisplabel.h"
#include "sailpainter.h"
#include <QMouseEvent>

/** Constructs a QLabel based view area.
 */
CSailDispLabel::CSailDispLabel( QWidget * parent )
        : QLabel( parent )
{
    // set the background to white
    QPalette pal = palette();
    pal.setColor( QPalette::Background, Qt::white );
    setPalette( pal );
}


/** Forces a redraw of the view area.
 */
void CSailDispLabel::redraw()
{
    update();
}


/** We received a mouse click.
 */
void CSailDispLabel::mousePressEvent ( QMouseEvent *event )
{
    if (event->button() == Qt::LeftButton)
    {
        setCenter(screenToLogical(event->pos().x(),event->pos().y()));
        redraw();
    }
}


/** We received a request to paint the drawing area.
 */
void CSailDispLabel::paintEvent( QPaintEvent * )
{
    // erase viewport
    CSailPainter painter( this );
    QRect rect = painter.viewport();
    painter.eraseRect(rect);

    // check if the window was resized since last redraw
    if ( wasResized )
    {
        setViewRect(painter.viewRect());
        wasResized = 0;
    }

    // set coordinate system to match the logical viewport
    painter.setWindow(getLogicalRect());
    painter.setFontSize(10, zoom);

    // draw the sail
    painter.draw(dispObject);

    // optionally draw labels
    if (drawLabels)
    {
        painter.drawLabels(dispObject);
    }
    painter.end();
}


/** The draw area has been resized.
 */
void CSailDispLabel::resizeEvent( QResizeEvent * )
{
    wasResized = 1;
}


/** We received a mouse wheel movement.
 */
void CSailDispLabel::wheelEvent( QWheelEvent *event)
{
    if (event->delta()>0)
        zoomIn();
    else
        zoomOut();
    redraw();
}
