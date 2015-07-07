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

#include "GestureHoldModeAction.h"

GestureHoldModeAction::GestureHoldModeAction() {
    rollSensitivity = 1; pitchSensitivity = 1; yawSensitivity = 1;
}

GestureHoldModeAction::GestureHoldModeAction(float rollSensitivity, float pitchSensitivity, float yawSensitivity) : 
    rollSensitivity(rollSensitivity), pitchSensitivity(pitchSensitivity), yawSensitivity(yawSensitivity)
{}

void GestureHoldModeAction::clearMap()
{
    actionMap.clear();
}

bool GestureHoldModeAction::addToActionMap(angleData ad, kybdCmds command)
{
    actionMap[ad.hash()] = command;

    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        return false; // something went wrong.
    }
    return true;
}

kybdCmds GestureHoldModeAction::getAction(angleData ad)
{
    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        return kybdCmds::NO_CMD;
    }

    return actionMap[ad.hash()];
}