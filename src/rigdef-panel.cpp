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

#include "rigdef-panel.h"
#include "sailreader-xml.h"
#include "sailworker.h"

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QGroupBox>
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
        txtBox.push_back(new QLineEdit(this));
    }

    QLabel *lblStaticX = new QLabel(this);
    lblStaticX->setText("x : ");
    QLabel *lblStaticY = new QLabel(this);
    lblStaticY->setText("y : ");
    QLabel *lblStaticZ = new QLabel(this);
    lblStaticZ->setText("z : ");

    QGridLayout* layout = new QGridLayout( this );
    layout->addWidget(lblStaticX, 0, 0);
    layout->addWidget(txtBox[0], 0, 1);
    layout->addWidget(lblStaticY, 0, 2);
    layout->addWidget(txtBox[1], 0, 3);
    layout->addWidget(lblStaticZ, 0, 4);
    layout->addWidget(txtBox[2], 0, 5);

}


/** Returns the vector corresponding to the user's input.
 */
CVector3d CVector3dWidget::getVector()
{
    CVector3d v;
    for (int i = 0; i < 3; i++)
        v[i] = txtBox[i]->text().toDouble();
    return v;
}


/** Set the input boxes from a given vector.
 *
 * @param v the new vector to display
 */
void CVector3dWidget::setVector(const CVector3d &v)
{
    for (unsigned int i = 0; i < 3; i++)
        txtBox[i]->setText(QString::number(v.getcoord(i)));
}


/** The constructor.
 *
 * @param parent the parent widget
 */
CRigSailWidget::CRigSailWidget(QWidget *parent)
        : QWidget(parent)
{
    grpInfo = new QGroupBox( 0 );
    QGridLayout* iLayout = new QGridLayout( grpInfo );

    lblFileStatic = new QLabel( grpInfo );
    iLayout->addWidget( lblFileStatic, 0, 0 );
    lblFile = new QLabel( grpInfo );
    iLayout->addWidget( lblFile, 0, 1 );
    btnReload = new QPushButton(grpInfo);
    iLayout->addWidget( btnReload, 0, 2);
    lblNameStatic = new QLabel( grpInfo );
    iLayout->addWidget( lblNameStatic, 1, 0 );
    txtName = new QLineEdit( grpInfo );
    iLayout->addWidget( txtName, 1, 1 );

    wdgOrigin = new CVector3dWidget(grpInfo);
    iLayout->addWidget(wdgOrigin, 2, 0, 1, 3);

    btnUpdate = new QPushButton(this);
    btnRemove = new QPushButton(this);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(grpInfo, 0, 0, 1, 3);

    layout->addWidget(btnRemove, 1, 0);
    layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 1, 1 );
    layout->addWidget(btnUpdate, 1, 2);

    languageChange();

    /* connect signals */
    connect( btnUpdate, SIGNAL( clicked() ), this, SLOT(slotUpdate()) );
    connect( btnReload, SIGNAL( clicked() ), this, SLOT(slotReload()) );
    connect( btnRemove, SIGNAL( clicked() ), this, SLOT(slotRemove()) );
}


/**
 * Sets the strings of the subwidgets using the current language.
 */
void CRigSailWidget::languageChange()
{
    grpInfo->setTitle( tr( "Sail information" ) );
    lblFileStatic->setText( tr("file") );
    lblNameStatic->setText( tr("name") );
    btnReload->setText( tr("Reload") );
    btnRemove->setText( tr("Remove") );
    btnUpdate->setText( tr("Update") );
}


/**
 * The rigsail changed, update widgets.
 *
 * @param newsail The new value of the rigsail
 */
void CRigSailWidget::setRigSail(const CRigSail& newsail)
{
    rigsail = newsail;
    lblFile->setText( rigsail.filename );
    txtName->setText( rigsail.title );
    wdgOrigin->setVector( rigsail.origin );
}


/**
 * The "reload" button was pressed, fire signalUpdate.
 */
void CRigSailWidget::slotReload()
{
    try
    {
        switch (rigsail.type)
        {
        case SAILDEF:
            (CPanelGroup&)rigsail = CSailWorker(CSailDefXmlReader().read(rigsail.filename)).makeSail();
            break;
        case SAIL3D:
            (CPanelGroup&)rigsail = CPanelGroupXmlReader("sail").read(rigsail.filename);
            break;
        }
        signalUpdate(rigsail);
    }
    catch (CException e)
    {
        QMessageBox::information(0,tr("error"), tr("There was an error reading from the selected file."));
    }
}


/**
 * The "remove" button was pressed, fire signalRemove.
 */
void CRigSailWidget::slotRemove()
{
    signalRemove();
}


/**
 * The "update" button was pressed, fire signalUpdate.
 */
void CRigSailWidget::slotUpdate()
{
    rigsail.title = txtName->text();
    rigsail.origin = wdgOrigin->getVector();
    signalUpdate(rigsail);
}



/**
 * The constructor.
 *
 * @param parent the parent widget
 */
CRigDefPanel::CRigDefPanel(QWidget *parent)
        : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout( this );
    tabs = new QTabWidget(this);
    layout->addWidget( tabs, 0, 0);
}


/**
 * Sets the strings of the subwidgets using the current language.
 */
void CRigDefPanel::languageChange()
{
    for ( unsigned int i = 0; i < sailwidget.size(); i++)
        sailwidget[i]->languageChange();
}


/**
 * We were passed a new rig definition, update the widgets.
 *
 * @param newdef The new rig definition
 */
void CRigDefPanel::setRigDef(const CRigDef& newdef)
{
    unsigned int i;
    for ( i = 0; i < sailwidget.size(); i++)
    {
        tabs->removeTab(i);
        delete sailwidget[i];
    }

    rigdef = newdef;
    sailwidget.resize(rigdef.rigsail.size());

    for ( i = 0; i < rigdef.rigsail.size(); i++)
    {
        sailwidget[i] = new CRigSailWidget(0);
        sailwidget[i]->setRigSail(rigdef.rigsail[i]);

        connect(sailwidget[i], SIGNAL(signalRemove()), this, SLOT(slotRemove()));
        connect(sailwidget[i], SIGNAL(signalUpdate(const CRigSail&)), this, SLOT(slotUpdate(const CRigSail&)));

        tabs->addTab(sailwidget[i], rigdef.rigsail[i].title);
    }

    if (sailwidget.size() > 0)
        tabs->setCurrentIndex(0);
}



/**
 * The user requested the removal of the current rigsail.
 */
void CRigDefPanel::slotRemove()
{
    int tabIndex = tabs->currentIndex();

    tabs->removeTab(tabIndex);
    delete sailwidget[tabIndex];
    sailwidget.erase(sailwidget.begin()+tabIndex);
    rigdef.rigsail.erase(rigdef.rigsail.begin()+tabIndex);
    signalUpdate(rigdef);
}


/**
 * The user updated the current rigsail.
 *
 * @param newsail The new rigsail
 */
void CRigDefPanel::slotUpdate(const CRigSail& newsail)
{
    int tabIndex = tabs->currentIndex();

    rigdef.rigsail[tabIndex] = newsail;
    tabs->setTabText(tabIndex, rigdef.rigsail[tabIndex].title);
    signalUpdate(rigdef);
}

