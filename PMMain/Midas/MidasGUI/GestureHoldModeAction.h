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

    angleData()
    {
        angleType = ROLL;
        anglePositive = true;
    }
    angleData(AngleType type, bool positive)
    {
        angleType = type;
        anglePositive = positive;
    }

    AngleType angleType;
    bool anglePositive;
} angleData;

class GestureHoldModeAction {
public:
    GestureHoldModeAction();
    GestureHoldModeAction(float rollSensitivity, float pitchSensitivity, float yawSensitivity);

    void clearMap();

    bool addToActionMap(angleData ad, kybdCmds command);

    kybdCmds getAction(angleData ad);

    void setRollSensitivity(float sensitivity) { rollSensitivity = sensitivity; }
    float getRollSensitivity() { return rollSensitivity; }

    void setPitchSensitivity(float sensitivity) { pitchSensitivity = sensitivity; }
    float getPitchSensitivity() { return pitchSensitivity; }

    void setYawSensitivity(float sensitivity) { yawSensitivity = sensitivity; }
    float getYawSensitivity() { return yawSensitivity; }

    void setActionType(HoldModeActionType type) { actionType = type; }
    HoldModeActionType getActionType() { return actionType; }

    void setIntervalLen(unsigned int len) { intervalLen = len; }
    unsigned int getIntervalLen() { return intervalLen; }

    void setVelocityIntervalLen(unsigned int len) { velocityIntervalLen = len; }
    unsigned int getVelocityIntervalLen() { return velocityIntervalLen; }

private:
    // Sensitivities indicate how many DEGREES are required to trigger ONE action
    // (stored in the actionMap)
    float rollSensitivity;
    float pitchSensitivity;
    float yawSensitivity;

    // defines functionality for observer
    HoldModeActionType actionType;

    // Vars required for INTERVAL_DELTA mode
    unsigned int intervalLen;
    unsigned int currIntervalCount;
    // Vars required for ABS_DELTA_VELOCITY mode
    unsigned int velocityIntervalLen;
    unsigned int velocityCurrIntervalCount;

    std::unordered_map<int, kybdCmds> actionMap;    
};

#endif /* GESTURE_HOLD_MODE_ACTION_HPP */