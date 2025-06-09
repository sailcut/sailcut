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

#ifndef SAILCUTQT_H
#define SAILCUTQT_H

#include <QApplication>
#include "prefs.h"

class QTranslator;
class QUrl;

/**
 *  Class representing a Sailcut application.
 */
class CSailApp : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList recentDocuments READ recentDocuments NOTIFY recentDocumentsChanged)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)

public:
    CSailApp(int &argc, char** argv);

    void readPrefs();
    void writePrefs();

    QString language() const;
    void setLanguage(const QString &language);

    QStringList recentDocuments() const;
    void addRecentDocument(const QString &filename);
    void removeRecentDocument(const QString &filename);

    QSize windowSize() const;
    void setWindowSize(const QSize &size);

public slots:
    void createBoat() const;
    void createHull() const;
    void createRig() const;
    void createSail() const;
    void open(const QString &filename);
    void showHandbook() const;

signals:
    void languageChanged();
    void recentDocumentsChanged();
    void windowSizeChanged();

private:
    void loadTranslation(const QString locale);

    /** the user's preferences */
    CPrefs prefs;

    /** the file in which the preferences are stored */
    QString prefsfile;

    /** translator for Qt's messages */
    QTranslator *transQt;
    /** transator for Sailcut's messages */
    QTranslator *transApp;
};

#endif
