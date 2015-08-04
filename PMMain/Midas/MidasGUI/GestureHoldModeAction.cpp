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
    rollSensitivity = DEFAULT_SENSITIVITY; pitchSensitivity = DEFAULT_SENSITIVITY; yawSensitivity = DEFAULT_SENSITIVITY;
    actionType = DEFAULT_ACTION_TYPE;
    intervalLen = DEFAULT_INTERVAL_LEN;
    intervalExecMultiplier = DEFAULT_INTERVAL_EXEC_MULTIPLIER;
    intervalMaxExecs = DEFAULT_INTERVAL_MAX_EXECS;
    velocityIntervalLen = DEFAULT_INTERVAL_LEN;
}

GestureHoldModeAction::GestureHoldModeAction(float rollSensitivity, float pitchSensitivity, float yawSensitivity) : 
    rollSensitivity(rollSensitivity), pitchSensitivity(pitchSensitivity), yawSensitivity(yawSensitivity)
{}

void GestureHoldModeAction::clean()
{
    actionMap.clear();
    actionType = DEFAULT_ACTION_TYPE;
    intervalLen = DEFAULT_INTERVAL_LEN;
    velocityIntervalLen = DEFAULT_INTERVAL_LEN;
    rollSensitivity = DEFAULT_SENSITIVITY;
    pitchSensitivity = DEFAULT_SENSITIVITY;
    yawSensitivity = DEFAULT_SENSITIVITY;
}

bool GestureHoldModeAction::addToActionMap(angleData ad, CommandData command)
{
    actionMap[ad.hash()] = command;

    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        return false; // something went wrong.
    }
    return true;
}

CommandData GestureHoldModeAction::getAction(angleData ad)
{
    if (actionMap.find(ad.hash()) == actionMap.end())
    {
        CommandData retVal;
        retVal.setType(commandType::NONE);
        return retVal;
    }

    return actionMap[ad.hash()];
}