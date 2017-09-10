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

#ifndef SAILTREEITEM_H
#define SAILTREEITEM_H

#include "sailcpp/panelgroup.h"

#include <QList>
#include <QVariant>

class QIcon;

class CSailTreeItem
{
public:
    CSailTreeItem(const QList<QVariant> &data, CSailTreeItem *parent = 0);
    CSailTreeItem(const CPanelGroup& data, QString name, CSailTreeItem *parent = 0);
    CSailTreeItem(const CPanel& data, QString name, CSailTreeItem *parent = 0);
    CSailTreeItem(const CPoint3d& data, QString name, CSailTreeItem *parent = 0);
    CSailTreeItem(const string& data, QString name, CSailTreeItem *parent = 0);

    /** Constructs a CSailTreeItem representing a vector of elements.
     *
     * @param v the vector
     * @param name the name of the vector
     * @param parent the parent node
     */
    template<class myType>
    CSailTreeItem(const vector<myType>& v, QString name, CSailTreeItem *parent)
    {
        parentItem = parent;
        itemData << name;
        itemIcon = NULL;
        for (unsigned int i = 0; i < v.size(); i++)
            appendChild(new CSailTreeItem(v[i], QString::number(i), this));
    }

    ~CSailTreeItem();

    void appendChild(CSailTreeItem *child);

    CSailTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QIcon icon() const;
    int row() const;
    CSailTreeItem *parent();

private:
    QList<CSailTreeItem*> childItems;
    QList<QVariant> itemData;
    const char **itemIcon;
    CSailTreeItem *parentItem;
};
#endif
