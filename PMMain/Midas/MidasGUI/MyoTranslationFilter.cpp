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

#define NOMINMAX
#include "MyoTranslationFilter.h"

#include "SharedCommandData.h"
#include "ControlState.h"
#include "MyoState.h"
#include "MainGUI.h"
#include "ProfileManager.h"
#include "BaseMeasurements.h"
#include "HoldModeObserver.h"
#include "ProfileCommon.h"
#include "myo\myo.hpp"
#include <iostream>

SettingsSignaller MyoTranslationFilter::settingsSignaller;

int sign(float val)
{
    if (val >= 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

MyoTranslationFilter::MyoTranslationFilter(ControlState* controlState, MyoState* myoState, MainGUI *mainGuiHandle)
    : controlStateHandle(controlState), myoStateHandle(myoState), previousMode(LOCK_MODE),
    pitch(0), prevPitch(0), deltaPitchDeg(0),
    yaw(0), prevYaw(0), deltaYawDeg(0),
    roll(0), prevRoll(0), deltaRollDeg(0),
    mainGui(mainGuiHandle)
{
    defaultGestHoldModeActionArr();

	Filter::setFilterError(filterError::NO_FILTER_ERROR);
	Filter::setFilterStatus(filterStatus::OK);
	Filter::clearOutput();

    if (mainGui)
    {
        mainGui->connectSignallerToSettingsDisplayer(&settingsSignaller);
    }

    hmo = NULL;
}

MyoTranslationFilter::~MyoTranslationFilter()
{
    delete hmo; hmo = NULL;
}

void MyoTranslationFilter::process()
{
    filterDataMap input = Filter::getInput();
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);
    Filter::clearOutput();

    filterDataMap output;

    handleQuatData(input, output);

    handleGyroData(input, output);

    handleArmData(input, output);

    handleXDirectionData(input, output);
}

void MyoTranslationFilter::handleQuatData(filterDataMap input, filterDataMap output)
{
    float quatX;
    float quatY;
    float quatZ;
    float quatW;

    // Require an entire quaternion in one input to process
    if (input.find(INPUT_QUATERNION_X) != input.end() &&
        input.find(INPUT_QUATERNION_Y) != input.end() &&
        input.find(INPUT_QUATERNION_Z) != input.end() &&
        input.find(INPUT_QUATERNION_W) != input.end())
    {
        boost::any valueX = input[INPUT_QUATERNION_X];
        boost::any valueY = input[INPUT_QUATERNION_Y];
        boost::any valueZ = input[INPUT_QUATERNION_Z];
        boost::any valueW = input[INPUT_QUATERNION_W];
        if (valueX.type() != typeid(float) ||
            valueY.type() != typeid(float) ||
            valueZ.type() != typeid(float) ||
            valueW.type() != typeid(float))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            quatX = boost::any_cast<float>(valueX);
            quatY = boost::any_cast<float>(valueY);
            quatZ = boost::any_cast<float>(valueZ);
            quatW = boost::any_cast<float>(valueW);

            roll = getRollFromQuaternion(quatX, quatY, quatZ, quatW, myoStateHandle->getArm(), myoStateHandle->getXDirection());
            pitch = getPitchFromQuaternion(quatX, quatY, quatZ, quatW, myoStateHandle->getArm(), myoStateHandle->getXDirection());
            yaw = getYawFromQuaternion(quatX, quatY, quatZ, quatW);
            int rollDeg = (int)(roll * (180 / M_PI));
            int prevRollDeg;

            myoStateHandle->pushRotation(myo::Quaternion<float>(quatX, quatY, quatZ, quatW));
            BaseMeasurements::getInstance().setCurrentAngles(roll, pitch, yaw);

            midasMode currMode = controlStateHandle->getMode();

            if (previousMode != currMode)
            {
                // update base angles for each new mode - invalidate 
                // BaseMeasurements until all updates are executed.
                BaseMeasurements::getInstance().setInvalid(true);
                BaseMeasurements::getInstance().updateBaseCursor();
                BaseMeasurements::getInstance().setBaseAngles(roll, pitch, yaw);
                BaseMeasurements::getInstance().setCurrentState(currMode);
                BaseMeasurements::getInstance().setInvalid(false);

                updateHoldModeObserver(currMode);
            }

            deltaRollDeg = radToDeg(calcRingDelta(roll, BaseMeasurements::getInstance().getBaseRoll()) - calcRingDelta(prevRoll, BaseMeasurements::getInstance().getBaseRoll())); // normalized to avoid overflow
            prevRoll = roll;
            prevRollDeg = (int)(prevRoll * (180 / M_PI));
            deltaPitchDeg = radToDeg(calcRingDelta(pitch, BaseMeasurements::getInstance().getBasePitch()) - calcRingDelta(prevPitch, BaseMeasurements::getInstance().getBasePitch())); // normalized to avoid overflow
            prevPitch = pitch;
            deltaYawDeg = radToDeg(calcRingDelta(yaw, BaseMeasurements::getInstance().getBaseYaw()) - calcRingDelta(prevYaw, BaseMeasurements::getInstance().getBaseYaw())); // normalized to avoid overflow
            prevYaw = yaw;

            unsigned int gestIdx;
            switch (currMode)
            {
            case MOUSE_MODE:
                performMouseModeFunc(output);
                break;
            case MOUSE_MODE2:
                performMouseModeFunc(output);
                break;
            case GESTURE_HOLD_ONE:
                gestIdx = 0;
                goto execute;
            case GESTURE_HOLD_TWO:
                gestIdx = 1;
                goto execute;
            case GESTURE_HOLD_THREE:
                gestIdx = 2;
                goto execute;
            case GESTURE_HOLD_FOUR:
                gestIdx = 3;
                goto execute;
            case GESTURE_HOLD_FIVE:
                gestIdx = 4;

            execute:
                //performHoldModeFunc(gestIdx, output); // Legacy. Not handled like this anymore.
                break;
            case KEYBOARD_MODE:
                performeKybdModeFunc(output);
                break;
            default:
                break;
            }

            if (currMode != MOUSE_MODE && currMode != MOUSE_MODE2)
            {
                if (previousMode == MOUSE_MODE || previousMode == MOUSE_MODE2)
                {
                    point mouseUnitVelocity = point(0, 0);
                    output[VELOCITY_INPUT] = mouseUnitVelocity;

                    vector2D noDelta = vector2D(0, 0);
                    output[DELTA_INPUT] = noDelta;
                }
            }

            Filter::setOutput(output);

            previousMode = controlStateHandle->getMode();
        }
    }
}

void MyoTranslationFilter::handleGyroData(filterDataMap input, filterDataMap output)
{
    float gyroX;
    float gyroY;
    float gyroZ;

    if (settingsSignaller.getUseGyroForCursorAccel())
    {

        // Require an entire quaternion in one input to process
        if (input.find(GYRO_DATA_X) != input.end() &&
            input.find(GYRO_DATA_Y) != input.end() &&
            input.find(GYRO_DATA_Z) != input.end())
        {
            boost::any valueX = input[GYRO_DATA_X];
            boost::any valueY = input[GYRO_DATA_Y];
            boost::any valueZ = input[GYRO_DATA_Z];
            if (valueX.type() != typeid(float) ||
                valueY.type() != typeid(float) ||
                valueZ.type() != typeid(float))
            {
                Filter::setFilterError(filterError::INVALID_INPUT);
                Filter::setFilterStatus(filterStatus::FILTER_ERROR);
            }
            else
            {
                gyroX = boost::any_cast<float>(valueX);
                gyroY = boost::any_cast<float>(valueY);
                gyroZ = boost::any_cast<float>(valueZ);

                normalizeGyroData(gyroX, gyroY, gyroZ);

                midasMode currMode = controlStateHandle->getMode();

                if (currMode == MOUSE_MODE || currMode == MOUSE_MODE2)
                {
                    // attempt 1
                    // update base angles for each new mode
                    //float gyroThresh = 0.075;
                    //float cursorBaseVerticalChange = abs(gyroY) > gyroThresh ? gyroY * CURSOR_GYRO_ACCEL_RATE : 0;// TODO - getBaseYChange(float gyroY) // gyroY axis correspon
                    //float cursorBaseHorizontalChange = abs(gyroZ) > gyroThresh ? -gyroZ * CURSOR_GYRO_ACCEL_RATE : 0;// TODO - getBaseXChange(float gyroZ)
                    //cursorBaseVerticalChange = pow(cursorBaseVerticalChange, 2);
                    //cursorBaseHorizontalChange = pow(cursorBaseHorizontalChange, 2);

                    // attempt 2
                    
                    float cursorBaseHorizontalChange = calcBaseHorizontalChange(gyroZ); 
                    
                    float cursorBaseVerticalChange = calcBaseVerticalChange(gyroY); 
                    

                    BaseMeasurements::getInstance().modifyBaseCursor(cursorBaseHorizontalChange, cursorBaseVerticalChange);
                }
            }
        }
    }
}

float MyoTranslationFilter::calcBaseHorizontalChange(float gyroZ)
{
    int cursorGyroPower = settingsSignaller.getCursorGyroPower();
    float cursorGyroScaleDown = settingsSignaller.getCursorGyroScaleDown();


    float horChange = abs(pow(gyroZ, cursorGyroPower)) / cursorGyroScaleDown * -sign(gyroZ);
    //horChange = std::min(horChange, 200.0f);

    return horChange;
}

float MyoTranslationFilter::calcBaseVerticalChange(float gyroY)
{
    int cursorGyroPower = settingsSignaller.getCursorGyroPower();
    float cursorGyroScaleDown = settingsSignaller.getCursorGyroScaleDown();

    float vertChange = abs(pow(gyroY, cursorGyroPower)) / cursorGyroScaleDown * sign(gyroY);
    //vertChange = std::min(vertChange, 200.0f);

    if (myoStateHandle->getXDirection() == XDirection::xDirectionTowardElbow)
    {
        vertChange = -vertChange;
    }
    return vertChange;
}

void MyoTranslationFilter::handleArmData(filterDataMap input, filterDataMap output)
{
    if (input.find(INPUT_ARM) != input.end())
    {
        boost::any value = input[INPUT_ARM];
        if (value.type() != typeid(myo::Arm))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            Arm arm = boost::any_cast<Arm>(input[INPUT_ARM]);
            myoStateHandle->setArm(arm);
        }
    }   
}

void MyoTranslationFilter::handleXDirectionData(filterDataMap input, filterDataMap output)
{
    if (input.find(INPUT_X_DIRECTION) != input.end())
    {
        boost::any value = input[INPUT_X_DIRECTION];
        if (value.type() != typeid(myo::XDirection))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            XDirection xDirection = boost::any_cast<XDirection>(input[INPUT_X_DIRECTION]);
            myoStateHandle->setXDirection(xDirection);
        }
    }
}

point MyoTranslationFilter::getMouseUnitVelocity(float pitch, float yaw)
{
	float deltaPitch = calcRingDelta(pitch, BaseMeasurements::getInstance().getBasePitch());
	float deltaYaw = calcRingDelta(yaw, BaseMeasurements::getInstance().getBaseYaw());

    int maxYawAngle = settingsSignaller.getYawMaxAngle();
    int maxPitchAngle = settingsSignaller.getPitchMaxAngle();
    float unitPitch = (deltaPitch >= 0) ? std::min(1.0f, deltaPitch / degToRad(maxPitchAngle)) : std::max(-1.0f, deltaPitch / degToRad(maxPitchAngle));
    float unitYaw = (deltaYaw >= 0) ? std::min(1.0f, deltaYaw / degToRad(maxYawAngle)) : std::max(-1.0f, deltaYaw / degToRad(maxYawAngle));

    return point((int) (unitYaw * 100), (int) (unitPitch * 100));
}

vector2D MyoTranslationFilter::getMouseDelta(float pitch, float yaw)
{
	float deltaPitch = calcRingDelta(pitch, BaseMeasurements::getInstance().getBasePitch());
	float deltaYaw = calcRingDelta(yaw, BaseMeasurements::getInstance().getBaseYaw());

    int maxYawAngle = settingsSignaller.getYawMaxAngle();
    int maxPitchAngle = settingsSignaller.getPitchMaxAngle();
    float relativePitch = (deltaPitch / degToRad(maxPitchAngle)) * 100;
    float relativeYaw = (deltaYaw / degToRad(maxYawAngle)) * 100;

	return vector2D((double)relativeYaw, (double)relativePitch);
}

float MyoTranslationFilter::getPitchFromQuaternion(float x, float y, float z, float w, Arm arm, XDirection xDirection)
{
    if (arm == Arm::armLeft && xDirection == XDirection::xDirectionTowardElbow)
    {
        return asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
    else if (arm == Arm::armRight && xDirection == XDirection::xDirectionTowardElbow)
    {
        return asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
    else if (arm == Arm::armLeft && xDirection == XDirection::xDirectionTowardWrist)
    {
        return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
    else if (arm == Arm::armRight && xDirection == XDirection::xDirectionTowardWrist)
    {
        return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    } 
    else
    {
        // Default - arbitrarily chosen.
        return -asin(std::max(-1.0f, std::min(1.0f, 2.0f * (w * y - z * x))));
    }
}

float MyoTranslationFilter::getYawFromQuaternion(float x, float y, float z, float w)
{
    // Indifferent to arm/xDirection, unlike pitch.
    return -atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
}

float MyoTranslationFilter::getRollFromQuaternion(float x, float y, float z, float w, Arm arm, XDirection xDirection)
{
    float roll = atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

    return xDirection == XDirection::xDirectionTowardWrist ? roll : -roll;
}

float MyoTranslationFilter::calcRingDelta(float current, float base)
{
    // Assert angles are within range of a circle [Pi, Pi)
    if (current >= M_PI || base >= M_PI || current < -M_PI || base < -M_PI)
    {
        return 0.0;
    }
    
    // normalize to 0 - 2Pi for this upcoming calculation.
    current += M_PI;
    base += M_PI;

    float delta = 0.0;
    if (current >= base)
    {
        if (current - base <= M_PI)
        {
            delta = current - base;
        }
        else
        {
            delta = -((2 * M_PI - current) + base);
        }
    }
    else
    {
        if (base - current <= M_PI)
        {
            delta = current - base;
        }
        else
        {
            delta = (2 * M_PI - base) + current;
        }
    }

    return delta;
}

void MyoTranslationFilter::normalizeGyroData(float &x, float &y, float &z)
{
    float** xRotationMatrix = myoStateHandle->getXRotationMatrix();

    // Perform matrix multiplication to effectively rotate the Gyro data into a normalized position where 
    // +z is vertical (wrt gravity) and +x is along arm
    float newX = x*xRotationMatrix[0][0] + y*xRotationMatrix[1][0] + z*xRotationMatrix[2][0];
    float newY = x*xRotationMatrix[0][1] + y*xRotationMatrix[1][1] + z*xRotationMatrix[2][1];
    float newZ = x*xRotationMatrix[0][2] + y*xRotationMatrix[1][2] + z*xRotationMatrix[2][2];

    x = newX;
    y = newY;
    z = newZ;

    return;
}

void MyoTranslationFilter::performHoldModeFunc(unsigned int holdNum, filterDataMap& outputToSharedCommandData)
{
//    CommandData command;
//    command.type = commandType::KYBRD_CMD;
//    command.name = "HoldMode Command";
//    command.action.kybd = kybdCmds::NO_CMD;
//
//    GestureHoldModeAction currentHoldModeAction = gestHoldModeAction[holdNum];
//    float thresh = .1;
//    
//    CommandData cd;
//    angleData ad;
//    bool tryAction = false;
//
//    ad.angleType = angleData::AngleType::ROLL;
//    if (deltaRollDeg > thresh)
//    {
//        tryAction = true;
//        ad.anglePositive = true;
//        
//    } 
//    else if (deltaRollDeg < -thresh)
//    {
//        tryAction = true;
//        ad.anglePositive = false;
//    }
//    if (tryAction)
//    {
//        command.action.kybd = kybdCmds((unsigned int)command.action.kybd | (unsigned int)currentHoldModeAction.getAction(ad));
//        outputToSharedCommandData[COMMAND_INPUT] = command;
//    }
//
//    tryAction = false;
//    ad.angleType = angleData::AngleType::PITCH;
//    if (deltaPitchDeg > thresh)
//    {
//        tryAction = true;
//        ad.anglePositive = true;
//    }
//    else if (deltaPitchDeg < -thresh)
//    {
//        tryAction = true;
//        ad.anglePositive = false;
//    }
//    if (tryAction)
//    {
//        command.action.kybd = kybdCmds((unsigned int)command.action.kybd | (unsigned int)currentHoldModeAction.getAction(ad));
//        outputToSharedCommandData[COMMAND_INPUT] = command;
//    }
//
//    tryAction = false;
//    ad.angleType = angleData::AngleType::YAW;
//    if (deltaYawDeg > thresh)
//    {
//        tryAction = true;
//        ad.anglePositive = true;
//    }
//    else if (deltaYawDeg < -thresh)
//    {
//        tryAction = true;
//        ad.anglePositive = false;
//    }
//    if (tryAction)
//    {
//        command.action.kybd = kybdCmds((unsigned int)command.action.kybd | (unsigned int)currentHoldModeAction.getAction(ad));
//        outputToSharedCommandData[COMMAND_INPUT] = command;
//    }
}

void MyoTranslationFilter::performMouseModeFunc(filterDataMap& outputToSharedCommandData)
{
    point mouseUnitVelocity = getMouseUnitVelocity(pitch, yaw);
    outputToSharedCommandData[VELOCITY_INPUT] = mouseUnitVelocity;

	outputToSharedCommandData[DELTA_INPUT] = getMouseDelta(pitch, yaw);;
}

void MyoTranslationFilter::performeKybdModeFunc(filterDataMap& outputToSharedCommandData)
{
    keyboardAngle myoAngle;
	
    point myoAnglePoint = getMouseUnitVelocity(pitch, yaw);
    unsigned int magnitude = sqrt(pow(myoAnglePoint.x, 2) + pow(myoAnglePoint.y, 2));
    myoAngle.ringThreshReached = false;
    if (magnitude > KEYBOARD_THRESH_MAG)
    {
        myoAngle.ringThreshReached = true;
    }
	
    // for debug only
    myoAngle.x = myoAnglePoint.x;
    myoAngle.y = myoAnglePoint.y;
	
    // TODO - verify/disprove this function 180 - (180.0 / M_PI) * atan2((double)myoAnglePoint.y, (double)myoAnglePoint.x);
    // using 90 instead of 180 *seems* to make it better, but then the upper left quadrant is unnaccessable.
    int myoAngleDegree = 90 - (180.0 / M_PI) * atan2((double)myoAnglePoint.y, (double)myoAnglePoint.x); // NEED to add section size/2 TODO
    while (myoAngleDegree < 0)
    {
        myoAngleDegree += 360;
    }
	
    myoAngle.angle = myoAngleDegree;
	
    outputToSharedCommandData[ANGLE_INPUT] = myoAngle;
}



// ******************************************************** HOLD_MODE CODE BELOW ******************************************************** //
bool MyoTranslationFilter::defaultGestHoldModeActionArr(void)
{
    // default hold mode actions incase profiles file is corrupted

//    bool initOkay = true;
//
//    angleData ad;
//    ad.angleType = angleData::AngleType::ROLL;
//    ad.anglePositive = true;
//    gestHoldModeAction[GESTURE_FIST].setRollSensitivity(5);
//    gestHoldModeAction[GESTURE_FIST].setActionType(ABS_DELTA_FINITE);
//    gestHoldModeAction[GESTURE_FIST].setIntervalLen(100);
//    gestHoldModeAction[GESTURE_FIST].setVelocityIntervalLen(100);
//    initOkay &= gestHoldModeAction[GESTURE_FIST].addToActionMap(ad, kybdCmds::VOLUME_UP);
//    ad.anglePositive = false;
//    initOkay &= gestHoldModeAction[GESTURE_FIST].addToActionMap(ad, kybdCmds::VOLUME_DOWN);
//
//    ad.angleType = angleData::AngleType::PITCH;
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setPitchSensitivity(2);
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setActionType(ABS_DELTA_VELOCITY);
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setIntervalLen(100);
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setVelocityIntervalLen(100);
//    ad.anglePositive = true;
//    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::UP_ARROW);
//    ad.anglePositive = false;
//    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::DOWN_ARROW);
//
//    ad.angleType = angleData::AngleType::YAW;
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setYawSensitivity(2);
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setActionType(ABS_DELTA_FINITE);
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setIntervalLen(100);
//    gestHoldModeAction[GESTURE_FINGERS_SPREAD].setVelocityIntervalLen(100);
//    ad.anglePositive = true;
//    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::RIGHT_ARROW);
//    ad.anglePositive = false;
//    initOkay &= gestHoldModeAction[GESTURE_FINGERS_SPREAD].addToActionMap(ad, kybdCmds::LEFT_ARROW);
//
//    ad.angleType = angleData::AngleType::YAW;
//    gestHoldModeAction[GESTURE_WAVE_IN].setYawSensitivity(10);
//    gestHoldModeAction[GESTURE_WAVE_IN].setActionType(ABS_DELTA_FINITE);
//    gestHoldModeAction[GESTURE_WAVE_IN].setIntervalLen(100);
//    gestHoldModeAction[GESTURE_WAVE_IN].setVelocityIntervalLen(100);
//    ad.anglePositive = false;
//    initOkay &= gestHoldModeAction[GESTURE_WAVE_IN].addToActionMap(ad, kybdCmds::UNDO);
//    ad.anglePositive = true;
//    initOkay &= gestHoldModeAction[GESTURE_WAVE_IN].addToActionMap(ad, kybdCmds::REDO);
//
//    return initOkay;

    return true;
}

void MyoTranslationFilter::unregisterHoldModeActions(void)
{
    for (int i = 0; i < NUM_GESTURES; i++)
    {
        gestHoldModeAction[i].clean();
    }
}

filterError MyoTranslationFilter::updateBasedOnProfile(ProfileManager& pm, std::string name)
{
    std::vector<profile>* profiles = pm.getProfiles();

    profile prof;
    bool foundProfile = false;
    for (int i = 0; i < profiles->size(); i++)
    {
        if (name == profiles->at(i).profileName)
        {
            prof = profiles->at(i);
            foundProfile = true;
        }
    }

    if (!foundProfile) return filterError::PROCESSING_ERROR;

    this->unregisterHoldModeActions();

    bool okay = true;
    angleData ad;
    for (std::vector<hold>::iterator it = prof.holds.begin(); it != prof.holds.end(); ++it)
    {
        int gestType;
        if (it->gesture == "fist")
        {
            gestType = GESTURE_FIST;
        }
        else if (it->gesture == "fingersSpread")
        {
            gestType = GESTURE_FINGERS_SPREAD;
        }
        else if (it->gesture == "doubleTap")
        {
            gestType = GESTURE_DOUBLE_TAP;
        }
        else if (it->gesture == "waveIn")
        {
            gestType = GESTURE_WAVE_IN;
        }
        else if (it->gesture == "waveOut")
        {
            gestType = GESTURE_WAVE_OUT;
        }

        gestHoldModeAction[gestType].setActionType(holdModeActionTypeMap[it->holdModeActionType]);
        gestHoldModeAction[gestType].setIntervalLen(it->intervalLen);
        gestHoldModeAction[gestType].setVelocityIntervalLen(it->intervalLen);

        for (std::vector<angleAction>::iterator angleIt = it->angles.begin(); angleIt != it->angles.end(); ++angleIt)
        {
            angleData ad;
            if (angleIt->type == "roll")
            {
                ad.angleType = angleData::AngleType::ROLL;
                gestHoldModeAction[gestType].setRollSensitivity(angleIt->sensitivity);
            }
            else if (angleIt->type == "pitch")
            {
                ad.angleType = angleData::AngleType::PITCH;
                gestHoldModeAction[gestType].setPitchSensitivity(angleIt->sensitivity);
            }
            else
            {
                ad.angleType = angleData::AngleType::YAW;
                gestHoldModeAction[gestType].setYawSensitivity(angleIt->sensitivity);
            }

            ad.anglePositive = true;
            CommandData posCmd = getCDFromCommand(angleIt->anglePositive);
            okay &= gestHoldModeAction[gestType].addToActionMap(ad, posCmd);
            ad.anglePositive = false;
            CommandData negCmd = getCDFromCommand(angleIt->angleNegative);
            okay &= gestHoldModeAction[gestType].addToActionMap(ad, negCmd);

            if (!okay)
            {
                throw new std::exception("registerHoldModeActionException");
            }
        }
    }

    return filterError::NO_FILTER_ERROR;
}

void MyoTranslationFilter::updateHoldModeObserver(midasMode currMode)
{

    if (currMode == GESTURE_HOLD_ONE ||
        currMode == GESTURE_HOLD_TWO ||
        currMode == GESTURE_HOLD_THREE ||
        currMode == GESTURE_HOLD_FOUR ||
        currMode == GESTURE_HOLD_FIVE)
    {
        if (hmo == NULL)
        {
            hmo = new HoldModeObserver(myoStateHandle, controlStateHandle->getSCD(), &gestHoldModeAction[gestHoldModeActionIdx(currMode)]);
            hmo->kickOffObserver();
        }
    }
    else
    {
        if (hmo != NULL)
        {
            hmo->kill(); hmo = NULL;
        }
    }
}
