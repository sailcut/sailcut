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

#include "formprint.h"
#include "printer.h"
#include "sailpainter.h"

#include <QBoxLayout>
#include <QPushButton>


/** Construct a new print preview label.
 */
CPrintLabel::CPrintLabel(const CPrinter *prt)
    : page(0), printer(prt)
{
    QPalette pal = palette();
    pal.setColor( QPalette::Background, Qt::white );
    setPalette( pal );
}


/** Print the current page.
 */
void CPrintLabel::paintEvent(QPaintEvent *)
{
    CTextPainter painter(this);
    printer->print(&painter, page);
}


/** Construct a new print preview dialog.
 */
CFormPrint::CFormPrint(const CPrinter *prt)
{
    setMinimumSize( QSize( 300, 220 ) );

    QVBoxLayout *layout = new QVBoxLayout(this);

    // create main widget
    label = new CPrintLabel(prt);
    layout->addWidget(label);

    // add the buttons 
    QHBoxLayout* buttons= new QHBoxLayout();
    buttons->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    buttonOk = new QPushButton( tr("&OK"), this );
    buttonOk->setDefault( TRUE );
    buttons->addWidget( buttonOk );
    buttonCancel = new QPushButton( tr("&Cancel"), this );
    buttons->addWidget( buttonCancel );
    layout->addLayout(buttons);

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}


