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

#ifndef PROFILES_DISPLAYER_H
#define PROFILES_DISPLAYER_H

#include <qwidget.h>
#include "MidasCommon.h"

class QVBoxLayout;
class ProfileDisplayer;

class ProfilesDisplayer : public QWidget
{
    Q_OBJECT

public:
    ProfilesDisplayer(int widgetWidth = PROF_INDICATOR_WIDTH,
        int widgetHeight = PROF_INDICATOR_HEIGHT, QWidget *parent = 0);
    ~ProfilesDisplayer();

    void addProfile(std::string profileName);

    void clearProfiles();

    QSize sizeHint() const;

    std::vector<ProfileDisplayer*> *getProfileWidgets() { return &profileWidgets; }

    void updateActiveProfiles();

private:
    void updateButtonsToHighlightActive();

    int indWidth, indHeight;
    QVBoxLayout *layout;
    std::vector<ProfileDisplayer*> profileWidgets; // profiles owned
    int currentlyActiveProfile;
};

#endif PROFILES_DISPLAYER_H