/*
    Copyright (C) 2015 Midas

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#include "ProfilesDisplayer.h"
#include "ProfileDisplayer.h"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpalette.h>
#include <qcolor.h> 
#include <QEvent.h>

ProfilesDisplayer::ProfilesDisplayer(int widgetWidth, int widgetHeight, QWidget *parent) : 
    DraggableWidget(parent, Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint), indWidth(widgetWidth), indHeight(widgetHeight), currentlyActiveProfile(0)
{
    setToolTip(tr("Drag the Profiles Displayer with the left mouse button."));
    setWindowTitle(tr("Profiles Displayer"));

    setWindowOpacity(1);
    QPalette pal;
    pal.setColor(QPalette::Background, MIDAS_GREY);
    setAutoFillBackground(true);
    setPalette(pal);

    layout = new QVBoxLayout(this);
    this->setLayout(layout);

    QFont timesFont("Times", 11, QFont::Bold, true);
    QLabel *title = new QLabel("Profiles");
    title->setFont(timesFont);
    layout->addWidget(title, 0, Qt::AlignCenter);
}
    

ProfilesDisplayer::~ProfilesDisplayer()
{
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        delete profileWidgets.at(i); profileWidgets.at(i) = NULL;
    }
    profileWidgets.clear();
}

QSize ProfilesDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void ProfilesDisplayer::addProfile(std::string profileName)
{
    ProfileDisplayer* displayer = new ProfileDisplayer(profileName, PROF_INDICATOR_WIDTH, PROF_INDICATOR_HEIGHT, this);
    this->profileWidgets.push_back(displayer);
    this->layout->addWidget(displayer);

    if (this->profileWidgets.size() == 1)
    {
        displayer->setActiveProfile(true);
        updateButtonsToHighlightActive();
    }
}

void ProfilesDisplayer::clearProfiles()
{
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        delete profileWidgets.at(i); profileWidgets.at(i) = NULL;
    }
    profileWidgets.clear();
}

void ProfilesDisplayer::resizeEvent(QResizeEvent *event)
{
    QRegion maskedRegion(0, 0, width(), height(), QRegion::Rectangle);
    setMask(maskedRegion);
}

void ProfilesDisplayer::updateActiveProfiles()
{
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        if (i != currentlyActiveProfile && profileWidgets.at(i)->getActiveProfile())
        {
            // found "new" active profile. Therefore it's the truly active one.
            profileWidgets.at(currentlyActiveProfile)->setActiveProfile(false);
            currentlyActiveProfile = i;
            updateButtonsToHighlightActive();
        }
    }
}

void ProfilesDisplayer::updateButtonsToHighlightActive()
{
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        if (i == currentlyActiveProfile)
        {
            profileWidgets.at(i)->setPalette(QPalette(MYO_BLUE));
        }
        else
        {
            profileWidgets.at(i)->setPalette(QPalette(MIDAS_GREY));
            //profileWidgets.at(i)->setPalette(QPalette(this->parent.getPalette().getColor()));
        }
    }
}