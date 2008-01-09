/*
 * Copyright (C) 1993-2007 Robert & Jeremy Laine
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

#ifndef SAILCUTQT_H
#define SAILCUTQT_H

#include <QApplication>
#include "prefs.h"

#if QT_VERSION >= 0x040200
#define HAVE_QDESKTOPSERVICES
#else
#undef HAVE_QDESKTOPSERVICES
#endif

#if QT_VERSION >= 0x040300
#define HAVE_QSVGGENERATOR
#else
#undef HAVE_QSVGGENERATOR
#endif

class QTranslator;
class QUrl;

/**
 *  Class representing a Sailcut application.
 */
class CSailApp : public QApplication
{
public:
    CSailApp(int &argc, char** argv);

    static QUrl findHandbook(const QString locale);
    void loadTranslation(const QString locale);
    void readPrefs();
    void writePrefs();

    /** the user's preferences */
    CPrefs prefs;

protected:
    /** the file in which the preferences are stored */
    QString prefsfile;

    /** translator for Qt's messages */
    QTranslator *transQt;
    /** transator for Sailcut's messages */
    QTranslator *transApp;
};

#endif
