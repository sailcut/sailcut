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
#include "sailpainter.h"
#include "sailprinter.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QMessageBox>
#include <QResizeEvent>


/** Construct a new print preview label.
 *
 * @param frm the parent print form
 */
CPrintLabel::CPrintLabel(CFormPrint *frm)
        : form(frm), resizing(false)
{
    QPalette pal = palette();
    pal.setColor( QPalette::Background, Qt::white );
    setPalette( pal );

    // set the initial size
    QRect print = form->printDevice.pageRect();
    real rprint = real(print.width()) / real(print.height());
    if (rprint > 1)
    {
        setMinimumSize( QSize( 375, 250 ) );
    }
    else
    {
        setMinimumSize( QSize( 250, 375 ) );
    }
}


/** Print the current page.
 */
void CPrintLabel::paintEvent(QPaintEvent *)
{
    QRect print = form->printDevice.pageRect();

    // erase viewport
    CTextPainter painter(this);
    QRect rect = painter.viewport();
    painter.eraseRect(rect);

    // draw print preview
    form->printEngine->print(&painter, page, form->printFontSize * real(heightMM()) / real(form->printDevice.heightMM()));
}


/** Receive a resize event.
 */
void CPrintLabel::resizeEvent (QResizeEvent *)
{
    QRect print = form->printDevice.pageRect();
    if (resizing || !print.width() || !print.height() || !widthMM() || !heightMM())
        return;

    resizing = true;
    real rprint = real(print.width()) / real(print.height());
    real rview = real(widthMM()) / real(heightMM());

    if (rview > rprint)
    {
        resize(int(real(width()) / real(widthMM()) * real(heightMM()) * rprint), height());
    }
    else
    {
        resize(width(), int(real(height()) / real(heightMM()) * real(widthMM()) / rprint));
    }
    resizing = false;
}


/** Set the current page.
 *
 * @param p new the page
 */
void CPrintLabel::setPage(int p)
{
    if ((p >= 0) && (p < form->printEngine->pages()))
    {
        page = p;
        update();
        form->labelPage->setText(QString::number(p+1) + " / " + QString::number(form->printEngine->pages()));
    }
}


/** Display the previous page.
 */
void CPrintLabel::slotPagePrev()
{
    setPage(page - 1);
}


/** Display the next page.
 */
void CPrintLabel::slotPageNext()
{
    setPage(page + 1);
}


/** Construct a new print preview dialog.
 *
 * @param engine
 * @param orientation
 */
CFormPrint::CFormPrint(const CPrinter *engine, enum QPrinter::Orientation orientation)
        : printEngine(engine), printFontSize(10)
{
    // initialise printer
    printDevice.setOrientation(orientation);
    printDevice.setFullPage(FALSE);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // create main widget
    label = new CPrintLabel(this);
    layout->addWidget(label);

    // add the buttons
    QHBoxLayout* buttons= new QHBoxLayout();
    buttonLeft = new QToolButton();
    buttonLeft->setArrowType(Qt::LeftArrow);
    buttons->addWidget(buttonLeft);
    labelPage = new QLabel();
    buttons->addWidget(labelPage);
    buttonRight = new QToolButton();
    buttonRight->setArrowType(Qt::RightArrow);
    buttons->addWidget(buttonRight);

    buttons->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    buttonOk = new QPushButton( tr("&Print"), this );
    buttonOk->setDefault( TRUE );
    buttons->addWidget( buttonOk );
    buttonCancel = new QPushButton( tr("&Cancel"), this );
    buttons->addWidget( buttonCancel );
    layout->addLayout(buttons);

    // signals and slots connections
    connect( buttonLeft, SIGNAL( clicked() ), label, SLOT( slotPagePrev() ) );
    connect( buttonRight, SIGNAL( clicked() ), label, SLOT( slotPageNext() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( slotPrint() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    label->setPage(0);

    // resize
    resize( QSize(minimumSizeHint()) );
}


/** Print out the current data.
 */
void CFormPrint::slotPrint()
{
    // try printing
    try
    {
        QPrintDialog printDialog(&printDevice);
        printDialog.setMinMax(1, printEngine->pages());
        if ( printDialog.exec() == QDialog::Accepted )
        {
            // determine which pages need to be printed
            int minPage, maxPage;
            switch (printDialog.printRange())
            {
            case QAbstractPrintDialog::PageRange:
                minPage = printDialog.fromPage();
                maxPage = printDialog.toPage();
                break;
            default:
                minPage = 1;
                maxPage = printEngine->pages();
                break;
            }

            // print the requested pages
            CTextPainter painter(&printDevice);
            for (int i = minPage - 1; i < maxPage; i ++)
            {
                if ( i >= 0 && i < printEngine->pages())
                {
                    if ( i > 0 )
                        printDevice.newPage();
                    printEngine->print(&painter, i, printFontSize);
                }
            }
        }
    }
    catch (CException e)
    {
        QMessageBox::information(NULL, tr("error"), tr("There was a printing error"));
    }
    accept();
}


