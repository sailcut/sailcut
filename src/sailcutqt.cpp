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

#include "sailcutqt.h"
#include <QDir>
#include <QFile>
#include <QTranslator>
#include <QUrl>
#include "sailwriter-xml.h"

using namespace std;

/**
 * Creates an new Sailcut application.
 *
 * @param argc
 * @param argv
 */
CSailApp::CSailApp(int &argc, char** argv) :
        QApplication(argc, argv)
{
    // create and install translators
    transApp = new QTranslator( this );
    transQt = new QTranslator( this );

    // the file containing the user's preferences
    prefsfile = QDir::home().filePath(".sailcutrc");
}


/**
 * Tries to locate the Sailcut Handbook.
 */
QUrl CSailApp::findHandbook(const QString locale)
{
    QStringList docdirs;
    QString handbook;

    QStringList locales(locale);
    if (locale != "en")
        locales.append("en");

    // when run from the build location
    docdirs.append("../doc");

    // when run from the installed location
    docdirs.append(SAILCUT_DOC_PATH);

    // look for handbook in different languages
    QDir appDir(applicationDirPath());
    for (int i = 0; i < locales.size(); i++)
    {
        for (int d = 0; d < docdirs.size(); d++)
        {
            handbook = appDir.absoluteFilePath(docdirs.at(d) + QDir::separator() + locales.at(i) + QDir::separator() + "index.html");
            if (QFile::exists(handbook))
                return QUrl::fromLocalFile(QFileInfo(handbook).absoluteFilePath());
        }
    }

    return QUrl();
}


/**
 * Loads translation strings for a given language.
 */
void CSailApp::loadTranslation(const QString locale)
{
    QStringList datadirs;

    // translation file for Qt
    transQt->load( QString( "qt_" ) + locale, "." );
    installTranslator(transQt);

    // translation file for application strings
    QString qm = QString("sailcut_") + locale;

    // when run from the build location
    datadirs.append("ts");

    // when run from the installed location
    datadirs.append(SAILCUT_DATA_PATH);

    int d = 0;
    QDir appDir(applicationDirPath());
    while ((d < datadirs.size()) &&
            (!transApp->load(qm, appDir.absoluteFilePath(datadirs.at(d)))))
    {
        d++;
    }
    installTranslator(transApp);
}


/**
 * Try to read the user's preferences
 */
void CSailApp::readPrefs()
{
    try
    {
        if (QFile(prefsfile).exists())
            prefs = CPrefsXmlWriter().read(prefsfile);
    }
    catch (read_error e)
    {
        cout << "CSailApp::readPrefs : could not read preferences" << endl;
    }
}


/**
 * Try to write the user's preferences.
 */
void CSailApp::writePrefs()
{
    try
    {
        CPrefsXmlWriter().write(prefs, prefsfile);
    }
    catch (write_error e)
    {
        cout << "CSailApp::writePrefs : could not write preferences" << endl;
    }
}
