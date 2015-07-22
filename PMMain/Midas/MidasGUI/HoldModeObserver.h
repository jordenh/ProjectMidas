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

class MyoState;
class GestureHoldModeAction;
class SharedCommandData;

class HoldModeObserver {
public:
    HoldModeObserver();
    HoldModeObserver(MyoState* myoState, SharedCommandData* scd, GestureHoldModeAction* actions, unsigned int callbackPeriod = DEFAULT_CALLBACK_PER);
    ~HoldModeObserver();

    void kickOffObserver();

    void setCallbackPeriod(unsigned int callbackPeriod) { this->callbackPeriod = callbackPeriod; }
    unsigned int getCallbackPeriod() { return callbackPeriod; }

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

    // defines actions of observer
    GestureHoldModeAction* actions; // not owned

    // Vars required for INTERVAL_DELTA mode
    unsigned int currIntervalCount;

    // Vars required for ABS_DELTA_FINITE mode
    // Executed counts can be + or -, indicating how many actions associated with each angle type have been executed.
    int currRollExecuted; 
    int currPitchExecuted; 
    int currYawExecuted; 

    // Vars required for ABS_DELTA_VELOCITY mode
    unsigned int velocityCurrIntervalCount;

    // used to stop thread
    bool commitSuicide;
};

#endif