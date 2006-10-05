/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 *
 * $Id: saildisplabel.h,v 1.8 2006/01/25 21:54:02 jeremy_laine Exp $
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

#ifndef SAILDISPLABEL_H
#define SAILDISPLABEL_H

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <QLabel>
#include "saildisp.h"

class QGridLayout;


/** A wireframe display area for a sail, based on a QLabel.
 *
 * @see CSailDispGL
 */
class CSailDispLabel : public QLabel, public CSailDisp
{
public:
    CSailDispLabel(QWidget *parent);

    void redraw();

protected:

    void mousePressEvent ( QMouseEvent * );
    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );
    void wheelEvent( QWheelEvent *event);
};

#endif
