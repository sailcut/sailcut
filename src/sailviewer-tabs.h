/*
 * Copyright (C) 1993-2018 Robert & Jeremy Laine
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

#ifndef SAILVIEWER_TABS_H
#define SAILVIEWER_TABS_H

#include <QTabWidget>
#include "sailviewer-panel.h"

/** Collection of tabs holding several CSailViewerPanel.
 */
class CSailViewerTabs : public QTabWidget
{
    Q_OBJECT

public:
    CSailViewerTabs(QWidget *parent);
    void addViewer(CSailViewerPanel *viewer);
    void setObject(const CPanelGroup &obj);

private slots:
    void languageChange();

public:
    /** the widgets of each view */
    std::vector<CSailViewerPanel*> panel;
};

#endif
