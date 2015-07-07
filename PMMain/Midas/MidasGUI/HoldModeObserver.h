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

#ifndef HOLD_MODE_OBSERVER_H
#define HOLD_MODE_OBSERVER_H

#include "MidasCommon.h"

#define DEFAULT_CALLBACK_PER 20
#define DEFAULT_INTERVAL_LEN 100
#define DEFAULT_ACTION_TYPE HoldModeActionType::ABS_DELTA_FINITE

class MyoState;
class GestureHoldModeAction;
class SharedCommandData;

class HoldModeObserver {
public:
    HoldModeObserver();
    HoldModeObserver(MyoState* myoState, SharedCommandData* scd, GestureHoldModeAction* actions, unsigned int callbackPeriod = DEFAULT_CALLBACK_PER, HoldModeActionType actionType = DEFAULT_ACTION_TYPE,
        unsigned int intervalLen = DEFAULT_INTERVAL_LEN, unsigned int velocityIntervalLen = DEFAULT_INTERVAL_LEN);
    ~HoldModeObserver();

    void kickOffObserver();

    void setCallbackPeriod(unsigned int callbackPeriod) { this->callbackPeriod = callbackPeriod; }
    unsigned int getCallbackPeriod() { return callbackPeriod; }

//    void setActionType(HoldModeActionType type) { actionType = type; }
//    HoldModeActionType getActionType() { return actionType; }
//
//    void setIntervalLen(unsigned int len) { intervalLen = len; }
//    unsigned int getIntervalLen() { return intervalLen; }
//
//    void setVelocityIntervalLen(unsigned int len) { velocityIntervalLen = len; }
//    unsigned int getVelocityIntervalLen() { return velocityIntervalLen; }

    void kill() { commitSuicide = true; }

private:
    // Thread with main logic of observer, alive for state duration,
    // with execution defined by callbackPeriod
    void observerThread();

    void handleIntervalDelta();
    void handleAbsDeltaFinite();
    void handleAbsDeltaVelocity();

    void executeCommands(kybdCmds rollCmd, unsigned int rollTicks, kybdCmds pitchCmd, unsigned int pitchTicks, kybdCmds yawCmd, unsigned int yawTicks);

    float prevRoll;
    float prevPitch;
    float prevYaw;

    MyoState* myoStateHandle;

    SharedCommandData* sharedCommandDataHandle;

    // defines frequency of observer.
    unsigned int callbackPeriod;

//    // defines functionality of observer
//    HoldModeActionType actionType;
    
    // defines actions of observer
    GestureHoldModeAction* actions; // not owned

//    // Vars required for INTERVAL_DELTA mode
//    unsigned int intervalLen;
    unsigned int currIntervalCount;

    // Vars required for ABS_DELTA_FINITE mode
    // Executed counts can be + or -, indicating how many actions associated with each angle type have been executed.
    int currRollExecuted; 
    int currPitchExecuted; 
    int currYawExecuted; 

//    // Vars required for ABS_DELTA_VELOCITY mode
//    unsigned int velocityIntervalLen;
    unsigned int velocityCurrIntervalCount;

    // used to stop thread
    bool commitSuicide;
};

#endif