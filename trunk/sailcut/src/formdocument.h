/*
 * Copyright (C) 1993-2009 Robert & Jeremy Laine
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

#ifndef FORMDOCUMENT_H
#define FORMDOCUMENT_H

#include <QWidget>
#include <vector>

#include "filewriter.h"
#include "sailviewer-tabs.h"

using namespace std;

// forward definitions
class CPrefs;
class QMenu;

#define T_KEYPRESS \
  void keyPressEvent(QKeyEvent * e) { \
    tabs->panel[tabs->currentIndex()]->keyPressEvent(e); \
  };

/** A dialog holding a Sailcut document.
 */
class CFormDocument : public QWidget
{
    Q_OBJECT

public:
    /** The constructor. */
    CFormDocument(CPrefs *myPrefs, QWidget *parent)
            : QWidget(parent), prefs(myPrefs)
    {};
    /** Open a file. */
    virtual bool open(const QString &newfile) = 0;
    /** Save to the current file. */
    virtual bool save() = 0;
    /** Save to a user-specified file. */
    virtual bool saveAs() = 0;

public:
    /** Extra submenus for the File menu */
    vector<QMenu*> extraFileMenus;
    /** Extra actions for the View menu */
    vector<QAction*> extraViewActions;
    /** The current filename. */
    QString filename;

protected:
    /** The user preferences. */
    CPrefs *prefs;
    /** An optional tabbed widget */
    CSailViewerTabs *tabs;
};


template <class deftype, class writertype>
class CFormDocumentTmpl : public CFormDocument
{
public:
    /** The constructor.
     *
     * @params myPrefs
     */
    CFormDocumentTmpl(CPrefs *myPrefs, QWidget *parent)
            : CFormDocument(myPrefs, parent)
    {}
    ;

    /**
     * Reads the definition from an XML file.
     */
    virtual bool open(const QString &newfile)
    {
        try
        {
            deftype newdef = writertype().read(newfile);
            setDef(newdef);
            filename = newfile;
            return true;
        }
        catch (read_error e)
        {
            writertype::readErrorMessage();
        }
        return false;
    };

    /**
     * Saves the definition to an XML file.
     */
    virtual bool save()
    {
        if ( filename.isEmpty() )
            return saveAs();

        // try writing to file, catch exception
        try
        {
            writertype().write(def, filename);
            return true;
        }
        catch (write_error e)
        {
            writertype::writeErrorMessage();
        }
        return false;
    };

    /**
     * Opens a dialog to select the XML to which the definition should be saved.
     */
    virtual bool saveAs()
    {
        QString newfile = writertype().writeDialog(def, filename);

        if ( !newfile.isNull() )
        {
            filename = newfile;
            return true;
        }
        return false;
    };

protected:
    virtual void setDef(const deftype& newdef) = 0;

protected:
    deftype def;
};

#endif
