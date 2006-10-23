/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
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

#include "sailtreeitem.h"

#include <QStringList>

CSailTreeItem::CSailTreeItem(const QList<QVariant> &data, CSailTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

 CSailTreeItem::~CSailTreeItem()
{
    qDeleteAll(childItems);
}

void CSailTreeItem::appendChild(CSailTreeItem *item)
{
    childItems.append(item);
}

CSailTreeItem *CSailTreeItem::child(int row)
{
    return childItems.value(row);
}

int CSailTreeItem::childCount() const
{
    return childItems.count();
}

int CSailTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant CSailTreeItem::data(int column) const
{
    return itemData.value(column);
}

CSailTreeItem *CSailTreeItem::parent()
{
    return parentItem;
}

int CSailTreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<CSailTreeItem*>(this));

    return 0;
}
