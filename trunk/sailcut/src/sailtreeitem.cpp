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

SailTreeItem::SailTreeItem(const QList<QVariant> &data, SailTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

 SailTreeItem::~SailTreeItem()
{
    qDeleteAll(childItems);
}

void SailTreeItem::appendChild(SailTreeItem *item)
{
    childItems.append(item);
}

SailTreeItem *SailTreeItem::child(int row)
{
    return childItems.value(row);
}

int SailTreeItem::childCount() const
{
    return childItems.count();
}

int SailTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant SailTreeItem::data(int column) const
{
    return itemData.value(column);
}

SailTreeItem *SailTreeItem::parent()
{
    return parentItem;
}

int SailTreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<SailTreeItem*>(this));

    return 0;
}
