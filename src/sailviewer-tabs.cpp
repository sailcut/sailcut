/*
 * Copyright (C) 1993-2008 Robert & Jeremy Laine
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

/**
 * Constructs a tabbed widget to hold sail viewers.
 *
 * @param parent the parent widget
 */
CSailViewerTabs::CSailViewerTabs(QWidget *parent)
        : QTabWidget(parent)
{
#ifdef HAVE_QTOPENGL
    addViewer(new CSailViewerPanel(NULL, SHADED, true));
#endif
    addViewer(new CSailViewerPanel(NULL, WIREFRAME, true));
}


/**
 * Adds a tab with a new sail viewer.
 */
void CSailViewerTabs::addViewer(CSailViewerPanel *viewer)
{
    // add viewer
    panel.push_back(viewer);
    // add tab
    addTab(viewer,"");
}


/**
 * Sets the strings of the subwidgets using the current
 * language.
 */
void CSailViewerTabs::languageChange()
{
    int tabidx = 0;
    for (unsigned int i = 0; i < panel.size(); i++)
        panel[i]->languageChange();
#ifdef HAVE_QTOPENGL
    setTabText(tabidx++, tr("shaded view"));
#endif
    setTabText(tabidx++, tr("wireframe view"));
}


/**
 * Change the displayed object.
 *
 * @param obj the new object to display
 */
void CSailViewerTabs::setObject(const CPanelGroup &obj)
{
    int tabidx = 0;
#ifdef HAVE_QTOPENGL
    panel[tabidx++]->setObject(obj);
#endif
    panel[tabidx++]->setObject(obj);
}
