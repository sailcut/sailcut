/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * 
 * $Id: formhelp.cpp,v 1.15 2006/01/25 21:54:00 jeremy_laine Exp $
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

#include "formhelp.h"

#include <QTextBrowser>
#include <QLayout>
#include <QStringList>
#include <QFileInfo>

#include "prefs.h"


/** Constructs a CFormHelp object.
 *
 * @param parent parent dialog
 * @param pref the user's preferences
 * @param handbook start page 
 */
CFormHelp::CFormHelp(QWidget *parent, CPrefs *pref, const QString& handbook)
        : QDialog(parent), prefs(pref)
{
    setModal(true);
    browser = new QTextBrowser( this );
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    QGridLayout* layout = new  QGridLayout(this);
    layout->addWidget(browser, 0, 0);

    QString hDir = QFileInfo(handbook).absolutePath();
    QString hFile = QFileInfo(handbook).fileName();
    browser->setSearchPaths(QStringList(hDir));
    browser->setSource(QString(hFile));

    languageChange();
    resize( QSize(prefs->helpWindowWidth,prefs->helpWindowHeight).expandedTo(minimumSizeHint()) );
}


/** This event is received when the user closes the dialog.
 */
void CFormHelp::closeEvent( QCloseEvent *e )
{
    prefs->helpWindowHeight = height();
    prefs->helpWindowWidth = width();
    QDialog::closeEvent(e);
}


/** Sets the strings of the subwidgets using the current
 *  language.
 */
void CFormHelp::languageChange()
{
    setWindowTitle( tr("Sailcut CAD Handbook") );
}
