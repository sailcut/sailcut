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
#include <QDebug>
#include <QFile>
#include <QLibraryInfo>
#include <QTranslator>
#include <QUrl>

#include "formboat.h"
#include "formhull.h"
#include "formpanelgroup.h"
#include "formrig.h"
#include "formsail.h"
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
 * Creates a new boat and shows it in a new window.
 */
void CSailApp::createBoat() const
{
    CFormMain *wnd = new CFormBoat;
    wnd->show();
}


/**
 * Creates a new hull and shows it in a new window.
 */
void CSailApp::createHull() const
{
    CFormMain *wnd = new CFormHull;
    wnd->show();
}


/**
 * Creates a new rig and shows it in a new window.
 */
void CSailApp::createRig() const
{
    CFormMain *wnd = new CFormRig;
    wnd->show();
}


/**
 * Creates a new sail and shows it in a new window.
 */
void CSailApp::createSail() const
{
    CFormMain *wnd = new CFormSail;
    wnd->show();
}


/**
 * Tries to locate the Sailcut Handbook.
 */
QUrl CSailApp::findHandbook() const
{
    const QString locale = prefs.language;

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


QString CSailApp::language() const
{
    return prefs.language;
}


void CSailApp::setLanguage(const QString &language)
{
    if (language != prefs.language) {
        prefs.language = language;
        loadTranslation(language);
        emit languageChanged();
    }
}


QSize CSailApp::windowSize() const
{
    return QSize(prefs.mainWindowWidth, prefs.mainWindowHeight);
}


void CSailApp::setWindowSize(const QSize &size)
{
    prefs.mainWindowHeight = size.height();
    prefs.mainWindowWidth = size.width();
}


/**
 * Loads translation strings for a given language.
 */
void CSailApp::loadTranslation(const QString locale)
{
    QStringList datadirs;

    // translation file for Qt
    removeTranslator(transQt);
    transQt->load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    installTranslator(transQt);

    // translation file for application strings
    removeTranslator(transApp);
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


void CSailApp::open(const QString &filename)
{
    CFormMain *wnd;
    if (CSailDefXmlWriter().isDocument(filename)) {
        wnd = new CFormSail;
    } else if (CHullDefXmlWriter().isDocument(filename)) {
        wnd = new CFormHull;
    } else if (CBoatDefXmlWriter().isDocument(filename)) {
        wnd = new CFormBoat;
    } else if (CRigDefXmlWriter().isDocument(filename)) {
        wnd = new CFormRig;
    } else if (CPanelGroupXmlWriter().isDocument(filename)) {
        wnd = new CFormPanelGroup;
    } else {
        //statusbar->showMessage( tr("unknown document type '%1'").arg(filename) );
        return;
    }

    if (wnd->load(filename))
        wnd->show();
    else
        wnd->deleteLater();
}


QStringList CSailApp::recentDocuments() const
{
    QStringList documents;
    for (unsigned int i = 0; i < prefs.mruDocuments.size(); ++i)
        documents.push_back(prefs.mruDocuments[i]);
    return documents;
}


/**
 * Puts an entry at the top of the Most Recently Used files.
 *
 * @param newEntry The entry to be added
 */
void CSailApp::addRecentDocument(const QString &filename)
{
    prefs.mruDocuments.touchEntry(filename);
    emit recentDocumentsChanged();
}


/**
 * Removes an entry from the Most Recently Used files.
 *
 * @param oldEntry The entry to be removed
 */
void CSailApp::removeRecentDocument(const QString &filename)
{
    prefs.mruDocuments.removeEntry(filename);
    emit recentDocumentsChanged();
}


/**
 * Try to read the user's preferences
 */
void CSailApp::readPrefs()
{
    try
    {
        if (QFile(prefsfile).exists()) {
            prefs = CPrefsXmlWriter().read(prefsfile);
            loadTranslation(prefs.language);
        }
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
