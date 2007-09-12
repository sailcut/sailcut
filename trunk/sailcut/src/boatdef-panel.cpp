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

#include "boatdef-panel.h"
#include "sailwriter-xml.h"
#include "hullworker.h"
#include "rigworker.h"
#include "sailworker.h"

#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTabWidget>
#include <QPushButton>


/** The constructor for a CVector3dWidget.
 *
 * @param parent the parent widget
 */
CVector3dWidget::CVector3dWidget(QWidget *parent)
        : QWidget(parent)
{
    for (int i = 0; i < 3; i++)
    {
        QSpinBox *spin = new QSpinBox(this);
        spin->setRange(0, 10000);
        spin->setSingleStep(10);
        spinBox.push_back(spin);
    }

    QLabel *lblStaticX = new QLabel(this);
    lblStaticX->setText("x");
    QLabel *lblStaticY = new QLabel(this);
    lblStaticY->setText("y");
    QLabel *lblStaticZ = new QLabel(this);
    lblStaticZ->setText("z");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(lblStaticX);
    layout->addWidget(spinBox[0], 1);
    layout->addWidget(lblStaticY);
    layout->addWidget(spinBox[1], 1);
    layout->addWidget(lblStaticZ);
    layout->addWidget(spinBox[2], 1);
}


/** Returns the vector corresponding to the user's input.
 */
CVector3d CVector3dWidget::getVector()
{
    CVector3d v;
    for (int i = 0; i < 3; i++)
        v[i] = spinBox[i]->value();
    return v;
}


/** Set the input boxes from a given vector.
 *
 * @param v the new vector to display
 */
void CVector3dWidget::setVector(const CVector3d &v)
{
    for (unsigned int i = 0; i < 3; i++)
        spinBox[i]->setValue(int(v.getcoord(i)));
}


/** The constructor.
 *
 * @param parent the parent widget
 */
CBoatElementWidget::CBoatElementWidget(QWidget *parent)
        : QWidget(parent)
{
    grpInfo = new QGroupBox( 0 );
    QGridLayout* iLayout = new QGridLayout( grpInfo );

    lblFileStatic = new QLabel( grpInfo );
    iLayout->addWidget( lblFileStatic, 0, 0 );
    lblFile = new QLabel( grpInfo );
    iLayout->addWidget( lblFile, 0, 1 );
    
    lblNameStatic = new QLabel( grpInfo );
    iLayout->addWidget( lblNameStatic, 1, 0 );
    txtName = new QLineEdit( grpInfo );
    iLayout->addWidget( txtName, 1, 1 );
    
    btnReload = new QPushButton(grpInfo);
    iLayout->addWidget( btnReload, 2, 1);

    wdgOrigin = new CVector3dWidget(grpInfo);
    iLayout->addWidget(wdgOrigin, 3, 0, 1, 3);
    //iLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding ), 4, 1 );
    
    // add button below frame
    btnUpdate = new QPushButton(this);
    btnRemove = new QPushButton(this);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(grpInfo, 0, 0, 1, 3);

    layout->addWidget(btnRemove, 1, 0);
    layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 1, 1 );
    layout->addWidget(btnUpdate, 1, 2);
    
    // pack everything upward
    layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding ), 2, 0 );

    languageChange();

    /* connect signals */
    connect( btnUpdate, SIGNAL( clicked() ), this, SLOT(slotUpdate()) );
    connect( btnReload, SIGNAL( clicked() ), this, SLOT(slotReload()) );
    connect( btnRemove, SIGNAL( clicked() ), this, SLOT(slotRemove()) );
}


/**
 * Sets the strings of the subwidgets using the current language.
 */
void CBoatElementWidget::languageChange()
{
    grpInfo->setTitle( tr( "Element information" ) );
    lblFileStatic->setText( tr("file") );
    lblNameStatic->setText( tr("name") );
    btnReload->setText( tr("Reload") );
    btnRemove->setText( tr("Remove") );
    btnUpdate->setText( tr("Update") );
}


/**
 * The boat element changed, update widgets.
 *
 * @param newelement The new value of the boat element
 */
void CBoatElementWidget::setElement(const CBoatElement& newelement)
{
    element = newelement;
    lblFile->setText( element.filename );
    txtName->setText( element.title );
    wdgOrigin->setVector( element.origin );
}


/**
 * The "reload" button was pressed, fire signalUpdate.
 */
void CBoatElementWidget::slotReload()
{
    try
    {
        switch (element.type)
        {
        case SAILDEF:
            (CPanelGroup&)element = CSailWorker(CSailDefXmlWriter().read(element.filename)).makeSail();
            break;
        case HULLDEF:
            (CPanelGroup&)element = CHullWorker(CHullDefXmlWriter().read(element.filename)).makeHull();
            break;
        case RIGDEF:
            (CPanelGroup&)element = CRigWorker(CRigDefXmlWriter().read(element.filename)).makeRig();
            break;
        case PANELGROUP:
            (CPanelGroup&)element = CPanelGroupXmlWriter().read(element.filename);
            break;
        }
        signalUpdate(element);
    }
    catch (CException e)
    {
        CSailDefXmlWriter::readErrorMessage();
    }
}


/**
 * The "remove" button was pressed, fire signalRemove.
 */
void CBoatElementWidget::slotRemove()
{
    signalRemove();
}


/**
 * The "update" button was pressed, fire signalUpdate.
 */
void CBoatElementWidget::slotUpdate()
{
    element.title = txtName->text();
    element.origin = wdgOrigin->getVector();
    signalUpdate(element);
}



/**
 * The constructor.
 *
 * @param parent the parent widget
 */
CBoatDefPanel::CBoatDefPanel(QWidget *parent)
        : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout( this );
    tabs = new QTabWidget(this);
    layout->addWidget( tabs, 0, 0);
}


/**
 * Sets the strings of the subwidgets using the current language.
 */
void CBoatDefPanel::languageChange()
{
    for ( unsigned int i = 0; i < elementwidget.size(); i++)
        elementwidget[i]->languageChange();
}


/**
 * We were passed a new boat definition, update the widgets.
 *
 * @param newdef The new boat definition
 */
void CBoatDefPanel::setDef(const CBoatDef& newdef)
{
    unsigned int i;
    for ( i = 0; i < elementwidget.size(); i++)
    {
        tabs->removeTab(i);
        delete elementwidget[i];
    }

    def = newdef;
    elementwidget.resize(def.element.size());

    for ( i = 0; i < def.element.size(); i++)
    {
        elementwidget[i] = new CBoatElementWidget(0);
        elementwidget[i]->setElement(def.element[i]);

        connect(elementwidget[i], SIGNAL(signalRemove()), this, SLOT(slotRemove()));
        connect(elementwidget[i], SIGNAL(signalUpdate(const CBoatElement&)), this, SLOT(slotUpdate(const CBoatElement&)));

        tabs->addTab(elementwidget[i], def.element[i].title);
    }

    if (elementwidget.size() > 0)
        tabs->setCurrentIndex(0);
}



/**
 * The user requested the removal of the current boat element.
 */
void CBoatDefPanel::slotRemove()
{
    int tabIndex = tabs->currentIndex();

    tabs->removeTab(tabIndex);
    delete elementwidget[tabIndex];
    elementwidget.erase(elementwidget.begin()+tabIndex);
    def.element.erase(def.element.begin()+tabIndex);
    signalUpdate(def);
}


/**
 * The user updated the current boat element.
 *
 * @param newelement The new boat element
 */
void CBoatDefPanel::slotUpdate(const CBoatElement& newelement)
{
    int tabIndex = tabs->currentIndex();

    def.element[tabIndex] = newelement;
    tabs->setTabText(tabIndex, def.element[tabIndex].title);
    signalUpdate(def);
}

