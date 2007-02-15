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

#include "sailviewer-panel.h"
#include "sailviewer-tabs.h"

#include <QLayout>


/**
 * Constructs a window to display a rig.
 *
 * @param myPrefs the user preferences
 * @param parent the parent widget
 */
CSailViewerTabs::CSailViewerTabs(QWidget *parent)
        : QTabWidget(parent)
{
}


/**
 * Creates the main widget
 */
void CSailViewerTabs::addViewer(const enumViewMode viewMode, bool show_sliders, bool show_labeling = true)
{
    // create viewers
    CSailViewerPanel *viewer = new CSailViewerPanel(this, viewMode, show_sliders, show_labeling);
    panel.push_back(viewer);
    
    // add tab
    addTab(viewer,"");
}


