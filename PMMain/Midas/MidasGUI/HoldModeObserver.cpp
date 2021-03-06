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

#include "HoldModeObserver.h"
#include "MyoState.h"
#include "BaseMeasurements.h"
#include "MyoTranslationFilter.h"
#include "SharedCommandData.h"
#include "AdvancedFilterPipeline.h"
#include "FilterKeys.h"
#include "CommandData.h"
#include <thread>

HoldModeObserver::HoldModeObserver()
{
    commitSuicide = false;
}

HoldModeObserver::~HoldModeObserver()
{
}

HoldModeObserver::HoldModeObserver(MyoState* myoState, SharedCommandData* scd, GestureHoldModeAction* actions, unsigned int callbackPeriod) :
    sharedCommandDataHandle(scd), myoStateHandle(myoState), actions(actions), callbackPeriod(callbackPeriod), currIntervalCount(0), velocityCurrIntervalCount(0), 
    // actionType(actionType), intervalLen(intervalLen),  velocityIntervalLen(velocityIntervalLen), 
    currRollExecuted(0), currPitchExecuted(0), currYawExecuted(0)
{
    prevRoll  = BaseMeasurements::getInstance().getCurrentRoll();
    prevPitch = BaseMeasurements::getInstance().getCurrentPitch();
    prevYaw   = BaseMeasurements::getInstance().getCurrentYaw();
    commitSuicide = false;
}

void HoldModeObserver::kickOffObserver()
{
    // setup callback thread that uses 'this" as 'this'
    std::thread callbackThread(&HoldModeObserver::observerThread, this);
    callbackThread.detach();
}

void HoldModeObserver::observerThread()
{
    std::chrono::milliseconds period(callbackPeriod);
    do {
        std::this_thread::sleep_for(period);
        
        if (commitSuicide)
            break;
        
        switch (actions->getActionType())
        {
        case INTERVAL_DELTA:
            handleIntervalDelta();
            break;
        case ABS_DELTA_FINITE:
            handleAbsDeltaFinite();
            break;
        case ABS_DELTA_VELOCITY:
            handleAbsDeltaVelocity();
            break;
        default:
            break;
        }

        prevRoll = BaseMeasurements::getInstance().getCurrentRoll();
        prevPitch = BaseMeasurements::getInstance().getCurrentPitch();
        prevYaw = BaseMeasurements::getInstance().getCurrentYaw();
    } while (true);

    delete this;
}

void HoldModeObserver::handleIntervalDelta()
{
    if (currIntervalCount >= actions->getIntervalLen())
    {
        currIntervalCount = 0;
        float deltaRollDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentRoll(), prevRoll));
        float deltaPitchDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentPitch(), prevPitch));
        float deltaYawDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentYaw(), prevYaw));

        int rollTicks = deltaRollDeg / actions->getRollSensitivity();
        int pitchTicks = deltaPitchDeg / actions->getPitchSensitivity();
        int yawTicks = deltaYawDeg / actions->getYawSensitivity();

        scaleIntervalExecs(rollTicks, pitchTicks, yawTicks);

        // if 0, will return associated negative cmd, but wont be executed (so its safe).
        CommandData rollCmd = actions->getAction(angleData(angleData::AngleType::ROLL, rollTicks > 0));
        CommandData pitchCmd = actions->getAction(angleData(angleData::AngleType::PITCH, pitchTicks > 0));
        CommandData yawCmd = actions->getAction(angleData(angleData::AngleType::YAW, yawTicks > 0));

        executeCommands(rollCmd, abs(rollTicks), pitchCmd, abs(pitchTicks), yawCmd, abs(yawTicks));
    }
    else
    {
        currIntervalCount += callbackPeriod;
    }
}

void HoldModeObserver::handleAbsDeltaFinite()
{
    float deltaRollDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentRoll(), BaseMeasurements::getInstance().getBaseRoll()));
    float deltaPitchDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentPitch(), BaseMeasurements::getInstance().getBasePitch()));
    float deltaYawDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentYaw(), BaseMeasurements::getInstance().getBaseYaw()));

    // TODO - decide if this is THE better method (its different than below)
    int desiredRollTicks = (deltaRollDeg / actions->getRollSensitivity()) - currRollExecuted;
    int desiredPitchTicks = (deltaPitchDeg / actions->getPitchSensitivity()) - currPitchExecuted;
    int desiredYawTicks = (deltaYawDeg / actions->getYawSensitivity()) - currYawExecuted;

    // normalize to 0, -1, or 1
    int rollTicks   = desiredRollTicks  == 0 ? 0 : (desiredRollTicks / abs(desiredRollTicks));
    int pitchTicks  = desiredPitchTicks == 0 ? 0 : (desiredPitchTicks / abs(desiredPitchTicks));
    int yawTicks    = desiredYawTicks   == 0 ? 0 : (desiredYawTicks / abs(desiredYawTicks));

    // if 0, will return associated negative cmd, but wont be executed (so its safe).
    CommandData rollCmd = actions->getAction(angleData(angleData::AngleType::ROLL, rollTicks > 0));
    CommandData pitchCmd = actions->getAction(angleData(angleData::AngleType::PITCH, pitchTicks > 0));
    CommandData yawCmd = actions->getAction(angleData(angleData::AngleType::YAW, yawTicks > 0));

    executeCommands(rollCmd, abs(rollTicks), pitchCmd, abs(pitchTicks), yawCmd, abs(yawTicks));

    currRollExecuted += rollTicks;
    currPitchExecuted += pitchTicks;
    currYawExecuted += yawTicks;

    // This code 'works', but seems to be suffering from sending too much data too quickly to the keyboard controller - therefore it's losing it's initial
    // desired properties of repeatability. Modifying above for a second attempt
//    float deltaRollDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentRoll(), BaseMeasurements::getInstance().getBaseRoll()));
//    float deltaPitchDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentPitch(), BaseMeasurements::getInstance().getBasePitch()));
//    float deltaYawDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentYaw(), BaseMeasurements::getInstance().getBaseYaw()));
//
//    // TODO - decide if this is a better method - this way will send (repeatably) the number of ticks all at once on each interval,
//    // but may be too fast? -- Alternative is to only send ONE cmd per interval and modify the currXXXExecuted vars iteratively. That was initial design, but 
//    // tryin this for now...
//    int rollTicks =  (deltaRollDeg / actions->getRollSensitivity())   - currRollExecuted;
//    int pitchTicks = (deltaPitchDeg / actions->getPitchSensitivity()) - currPitchExecuted;
//    int yawTicks =   (deltaYawDeg / actions->getYawSensitivity())     - currYawExecuted;
//
//    // if 0, will return associated negative cmd, but wont be executed (so its safe).
//    kybdCmds rollCmd = actions->getAction(angleData(angleData::AngleType::ROLL, rollTicks > 0));
//    kybdCmds pitchCmd = actions->getAction(angleData(angleData::AngleType::PITCH, pitchTicks > 0));
//    kybdCmds yawCmd = actions->getAction(angleData(angleData::AngleType::YAW, yawTicks > 0));
//
//    executeCommands(rollCmd, abs(rollTicks), pitchCmd, abs(pitchTicks), yawCmd, abs(yawTicks));
//
//    currRollExecuted = (deltaRollDeg / actions->getRollSensitivity());
//    currPitchExecuted = (deltaPitchDeg / actions->getPitchSensitivity());
//    currYawExecuted = (deltaYawDeg / actions->getYawSensitivity());
}

void HoldModeObserver::handleAbsDeltaVelocity()
{
    if (velocityCurrIntervalCount >= actions->getVelocityIntervalLen())
    {
        velocityCurrIntervalCount = 0;

        float deltaRollDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentRoll(), BaseMeasurements::getInstance().getBaseRoll()));
        float deltaPitchDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentPitch(), BaseMeasurements::getInstance().getBasePitch()));
        float deltaYawDeg = MyoTranslationFilter::radToDeg(MyoTranslationFilter::calcRingDelta(BaseMeasurements::getInstance().getCurrentYaw(), BaseMeasurements::getInstance().getBaseYaw()));

        int rollTicks = deltaRollDeg / actions->getRollSensitivity();
        int pitchTicks = deltaPitchDeg / actions->getPitchSensitivity();
        int yawTicks = deltaYawDeg / actions->getYawSensitivity();

        scaleIntervalExecs(rollTicks, pitchTicks, yawTicks);

        // if 0, will return associated negative cmd, but wont be executed (so its safe).
        CommandData rollCmd = actions->getAction(angleData(angleData::AngleType::ROLL, rollTicks > 0));
        CommandData pitchCmd = actions->getAction(angleData(angleData::AngleType::PITCH, pitchTicks > 0));
        CommandData yawCmd = actions->getAction(angleData(angleData::AngleType::YAW, yawTicks > 0));

        executeCommands(rollCmd, abs(rollTicks), pitchCmd, abs(pitchTicks), yawCmd, abs(yawTicks));
    }
    else
    {
        velocityCurrIntervalCount += callbackPeriod;
    }
}

void HoldModeObserver::scaleIntervalExecs(int& rollTicks, int& pitchTicks, int& yawTicks)
{
    rollTicks *= actions->getIntervalExecMultiplier();
    pitchTicks *= actions->getIntervalExecMultiplier();
    yawTicks *= actions->getIntervalExecMultiplier();

    rollTicks = rollTicks > 0 ? min(rollTicks, actions->getIntervalMaxExecs()) : max(rollTicks, - (int) actions->getIntervalMaxExecs());
    pitchTicks = pitchTicks > 0 ? min(pitchTicks, actions->getIntervalMaxExecs()) : max(pitchTicks, - (int) actions->getIntervalMaxExecs());
    yawTicks = yawTicks > 0 ? min(yawTicks, actions->getIntervalMaxExecs()) : max(yawTicks, - (int) actions->getIntervalMaxExecs());
}

// LEGACY - only used to handle kybd commands
void HoldModeObserver::executeCommands(kybdCmds rollCmd, unsigned int rollTicks, kybdCmds pitchCmd, unsigned int pitchTicks, kybdCmds yawCmd, unsigned int yawTicks)
{
    AdvancedFilterPipeline pipe;
    pipe.registerFilterAtDeepestLevel(sharedCommandDataHandle);
    CommandData cmd;
    cmd.setType(commandType::KYBRD_CMD);
    cmd.setName("Hold Command");
    for (int i = 0; i < rollTicks && rollCmd != kybdCmds::NO_CMD; i++)
    {
        filterDataMap input;
        cmd.setActionKybd(rollCmd);
        input[COMMAND_INPUT] = cmd;
        pipe.startPipeline(input);
    }
    for (int i = 0; i < pitchTicks && pitchCmd != kybdCmds::NO_CMD; i++)
    {
        filterDataMap input;
        cmd.setActionKybd(pitchCmd);
        input[COMMAND_INPUT] = cmd;
        pipe.startPipeline(input);
    }
    for (int i = 0; i < yawTicks && yawCmd != kybdCmds::NO_CMD; i++)
    {
        filterDataMap input;
        cmd.setActionKybd(yawCmd);
        input[COMMAND_INPUT] = cmd;
        pipe.startPipeline(input);
    }
}

void HoldModeObserver::executeCommands(CommandData rollCmd, unsigned int rollTicks, CommandData pitchCmd, unsigned int pitchTicks, CommandData yawCmd, unsigned int yawTicks)
{
    AdvancedFilterPipeline pipe;
    pipe.registerFilterAtDeepestLevel(sharedCommandDataHandle);
    for (int i = 0; i < rollTicks && rollCmd.getType() != commandType::NONE; i++)
    {
        filterDataMap input;
        input[COMMAND_INPUT] = rollCmd;
        pipe.startPipeline(input);
    }
    for (int i = 0; i < pitchTicks && pitchCmd.getType() != commandType::NONE; i++)
    {
        filterDataMap input;
        input[COMMAND_INPUT] = pitchCmd;
        pipe.startPipeline(input);
    }
    for (int i = 0; i < yawTicks && yawCmd.getType() != commandType::NONE; i++)
    {
        filterDataMap input;
        input[COMMAND_INPUT] = yawCmd;
        pipe.startPipeline(input);
    }
}