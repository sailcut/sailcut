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

#include "sailtreeitem.h"
#include "point.xpm"
#include "panel.xpm"
#include "panelgroup.xpm"
#include <QStringList>
#include <QIcon>

CSailTreeItem::CSailTreeItem(const QList<QVariant> &data, CSailTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
    itemIcon = NULL;
}

CSailTreeItem::CSailTreeItem(const CPanelGroup &data, QString name, CSailTreeItem *parent)
{
    parentItem = parent;
    itemData << name;
    itemIcon = panelgroup_xpm;
    appendChild(new CSailTreeItem(data.title, "title", this));
    appendChild(new CSailTreeItem(std::vector<CPanel>(data), "panel", this));
    if (data.child.size() > 0)
        appendChild(new CSailTreeItem(data.child, "child", this));
}

CSailTreeItem::CSailTreeItem(const CPanel &data, QString name, CSailTreeItem *parent)
{
    parentItem = parent;
    itemData << name;
    itemIcon = panel_xpm;
    appendChild(new CSailTreeItem(data.left, "left", this));
    appendChild(new CSailTreeItem(data.right, "right", this));
    appendChild(new CSailTreeItem(data.top, "top", this));
    appendChild(new CSailTreeItem(data.bottom, "bottom", this));
}

CSailTreeItem::CSailTreeItem(const CPoint3d &data, QString name, CSailTreeItem *parent)
{
    parentItem = parent;
    itemIcon = point_xpm;
    itemData << name << data.x() << data.y() << data.z();
}

CSailTreeItem::CSailTreeItem(const std::string &data, QString name, CSailTreeItem *parent)
{
    parentItem = parent;
    itemIcon = NULL;
    itemData << name << QString::fromStdString(data);
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

QIcon CSailTreeItem::icon() const
{
    if (itemIcon != NULL)
        return QIcon(QPixmap(itemIcon));
    else
        return QIcon();
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
