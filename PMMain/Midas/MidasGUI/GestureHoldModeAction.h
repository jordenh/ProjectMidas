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
#include "CommandData.h"
#include <unordered_map>

#define DEFAULT_SENSITIVITY 1
#define DEFAULT_INTERVAL_LEN 100
#define DEFAULT_INTERVAL_EXEC_MULTIPLIER 1
#define DEFAULT_INTERVAL_MAX_EXECS 10
#define DEFAULT_ACTION_TYPE HoldModeActionType::ABS_DELTA_FINITE

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

    void clean();

    bool addToActionMap(angleData ad, CommandData command);

    CommandData getAction(angleData ad);

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

    void setIntervalExecMultiplier(unsigned int val) { intervalExecMultiplier = val; }
    unsigned int getIntervalExecMultiplier() { return intervalExecMultiplier; }

    void setIntervalMaxExecs(unsigned int val) { intervalMaxExecs = val; }
    unsigned int getIntervalMaxExecs() { return intervalMaxExecs; }

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
    // Vars required for ABS_DELTA_VELOCITY mode
    unsigned int velocityIntervalLen;

    unsigned int intervalExecMultiplier;
    unsigned int intervalMaxExecs;

    std::unordered_map<int, CommandData> actionMap;
};

#endif /* GESTURE_HOLD_MODE_ACTION_HPP */