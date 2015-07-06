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

#ifndef GESTURE_HOLD_MODE_ACTION_HPP
#define GESTURE_HOLD_MODE_ACTION_HPP

#include "MidasCommon.h"
#include <unordered_map>

typedef struct angleData {
    bool operator==(const angleData& ad)
    {
        if (this->anglePositive == ad.anglePositive &&
            this->angleType == ad.angleType)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int hash()
    {
        return 4 * angleType + (anglePositive ? 1 : 0);
    }
    
    enum AngleType {
        ROLL,
        PITCH,
        YAW
    };
    AngleType angleType;
    bool anglePositive;
} angleData;

class GestureHoldModeAction {
public:
    GestureHoldModeAction();
    GestureHoldModeAction(unsigned int minIntervalLen);

    void clearMap();

    bool addToActionMap(angleData ad, kybdCmds command);

    kybdCmds getAction(angleData ad);

private:
    // TODO - actually care about this type. In the meantime, coding Interval_delta since it's easiest.
    // enum holdModeActionType {
    //     INTERVAL_DELTA, // if in a given interval a signal is +/-, an action will ensue
    //     ABSOLUTE_DELTA_NUM_SIGS, // Based on the delta, ensure the number of +/- actions is at a certain value (repeatable change)
    //     ABSOLUTE_DELTA_VELOCITY // Based on delta, continue spewing actions
    // };

    std::unordered_map<int, kybdCmds> actionMap;

    unsigned int minIntervalLen; // between sampling signals and sending actions // TODO - make this useful
    
};

#endif /* GESTURE_HOLD_MODE_ACTION_HPP */