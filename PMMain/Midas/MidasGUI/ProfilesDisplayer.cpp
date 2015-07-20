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
#include <qpalette.h>
#include <qcolor.h> 
#include <QEvent.h>

ProfilesDisplayer::ProfilesDisplayer(int widgetWidth, int widgetHeight, QWidget *parent) : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight),
    currentlyActiveProfile(0)
{
    layout = new QVBoxLayout(this);
    this->setLayout(layout);
}
    

ProfilesDisplayer::~ProfilesDisplayer()
{
    for (int i = 0; i < profileWidgets.size(); i++)
    {
        delete profileWidgets.at(i); profileWidgets.at(i) = NULL;
    }
    profileWidgets.clear();
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

QSize ProfilesDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
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
            profileWidgets.at(i)->setPalette(QPalette(QColor(0, 188, 223)));
        }
        else
        {
            profileWidgets.at(i)->setPalette(QPalette(QColor(205, 205, 193)));
            //profileWidgets.at(i)->setPalette(QPalette(this->parent.getPalette().getColor()));
        }
    }
}