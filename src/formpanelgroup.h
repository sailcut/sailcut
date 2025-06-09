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

#ifndef FORMPANELGROUP_H
#define FORMPANELGROUP_H

#include "formmain.h"
#include "sailwriter-xml.h"

// forward definitions
class QTreeView;

/** Dialog holding a sail.
 */
class CFormPanelGroup : public CFormMain
{
    Q_OBJECT

public:
    CFormPanelGroup(QWidget *parent = 0);

protected:
    QString getSaveFileName(const QString &filename)
    { return writer.getSaveFileName(filename); };
    bool read(const QString &filename);
    bool write(const QString &filename);

private slots:
    void languageChange();

private:
    void setDef(const CPanelGroup& newdef);
    void setupMainWidget();

    CPanelGroup def;
    CPanelGroupXmlWriter writer;

    /** the tree view */
    QTreeView *treeview;
};

#endif
