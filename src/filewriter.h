/*
 * Copyright (C) 1993-2017 Robert & Jeremy Laine
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

#ifndef FILEWRITER_H
#define FILEWRITER_H

#ifdef WIN32
#define CRLF   endl
#else
#define CRLF "\r\n"
#endif

#include <iostream>
#include <stdexcept>

#include <QFileDialog>
#include <QMessageBox>

using namespace std;

class read_error : public runtime_error
{
public:
    read_error(const string &message) : runtime_error(message)
    {
        cout << what() << endl;
    }
};

class write_error : public runtime_error
{
public:
    write_error(const string &message) : runtime_error(message)
    {
        cout << what() << endl;
    }
};

/** This is a generic class used as the base for various file
 *  input and output modules.
 */
template <class objtype>
class CFileWriter : public QObject
{
public:
    /** The constructor.
     *
     * @param ext the file extension
     * @param desc description of the file type
     */
    CFileWriter(const QString &ext, const QString &desc)
        : _ext(ext)
        , _desc(desc)
    {};

    /** Return the file extension associated with this CFileWriter.
     */
    QString fileExtension() const
    {
        return _ext;
    };

    QString getOpenFileName(const QString &filename) const
    {
        return QFileDialog::getOpenFileName(0, tr("Open"), QFileInfo(filename).absolutePath(), _desc + " (*" + _ext + ")");
    };

    QString getSaveFileName(const QString &filename) const
    {
        QString newfilename = QFileDialog::getSaveFileName(0, tr("Save"), filename, _desc + " (*" + _ext + ")");
        if (!newfilename.isNull() && newfilename.right(_ext.length()).toLower() != _ext)
            newfilename += _ext;
        return newfilename;
    };

    /** Perform the actual reading operation, may be overriden
     *  to provide this functionality.
     */
    virtual const objtype read(const QString &) const
    {
        throw logic_error("Reading is not supported for this file type.");
    };


    /** Display a dialog then read file.
     *
     * @param dest the object we read to
     * @param filename initial file name
     */
    QString readDialog(objtype &dest, const QString &filename = "") const
    {
        const QString newfilename = getOpenFileName(filename);
        if (!newfilename.isNull()) {
            try {
                dest = read(newfilename);
            } catch (read_error e) {
                readErrorMessage();
                return QString::null;
            }
        }
        return newfilename;
    };


    /** Show an error message indicating that reading failed.
     */
    static void readErrorMessage()
    {
        QMessageBox::information(0, tr("error"), tr("There was an error reading from the selected file."));
    };


    /** Perform the actual writing operation, must be overriden.
     */
    virtual void write(const objtype &, const QString &) const = 0;


    /** Opens of a dialog to ask for a filename
     *  then writes to a file.
     *
     *  @param obj The object to write.
     *  @param filename The filename to start off with (default = "")
     */
    QString writeDialog(const objtype &obj, const QString &filename = QString::null) const
    {
        const QString newfilename = getSaveFileName(filename);
        if (!newfilename.isNull()) {
            try {
                write(obj, newfilename);
            } catch (write_error e) {
                writeErrorMessage();
                return QString::null;
            }
        }
        return newfilename;
    };


    /** Show an error message indicating that writing failed.
     */
    static void writeErrorMessage()
    {
        QMessageBox::information(0, tr("error"), tr("There was an error writing to the selected file."));
    };


    /**
     * Returns whether the given file is associated with this CFileWriter.
     */
    bool isDocument(QString filename) const
    {
        return (filename.right(_ext.length()).toLower() == _ext);
    };

protected:
    /** file extension */
    QString _ext;
    /** description of the file type */
    QString _desc;
};


#endif
