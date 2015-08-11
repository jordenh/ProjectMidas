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

#include "SettingsSignaller.h"
#include <math.h>

SettingsSignaller::SettingsSignaller(QObject *parent, unsigned int yawMaxAngle, unsigned int pitchMaxAngle, buzzFeedbackMode buzzFeedback,
    bool useGyroForCursorAccel, bool removeGyroOnHoldMouse, double cursorGyroPower, double cursorGyroScaleDown)
    : QObject(parent), yawMaxAngle(yawMaxAngle), pitchMaxAngle(pitchMaxAngle), buzzFeedback(buzzFeedback),
    useGyroForCursorAccel(useGyroForCursorAccel), cursorGyroPower(cursorGyroPower), cursorGyroScaleDown(cursorGyroScaleDown), useEMGImpulse(false), 
    desiredXRotation(M_PI), holdLength(DEFAULT_REQ_HOLD_TIME), removeGyroOnHoldMouse(removeGyroOnHoldMouse)
{
}

SettingsSignaller::~SettingsSignaller()
{

}

void SettingsSignaller::handleSliderValues(unsigned int yawMaxAngle, unsigned int pitchMaxAngle)
{
    this->yawMaxAngle = yawMaxAngle;
    this->pitchMaxAngle = pitchMaxAngle;
}

void SettingsSignaller::handleBuzzFeedbackChange(unsigned int buzzMode)
{
    this->buzzFeedback = (buzzFeedbackMode)buzzMode;
}

void SettingsSignaller::hanldeGyroPowerValueChanged(int val)
{
    this->cursorGyroPower = val;
}

void SettingsSignaller::hanldeGyroScaleDownValueChanged(double val)
{
    this->cursorGyroScaleDown = val;
}

void SettingsSignaller::handleUseGyroForCursorAccelChanged(bool val)
{
    this->useGyroForCursorAccel = val;
}

void SettingsSignaller::handleRemoveGyroOnHoldMouseChanged(bool val)
{
    this->removeGyroOnHoldMouse = val;
}

void SettingsSignaller::handleUseEmgImpulse(bool val)
{
    this->useEMGImpulse = val;
}

void SettingsSignaller::handleHelpLevelChanged(int helpLevelEnum)
{
    this->midasHelpLevel = (helpLevel)helpLevelEnum;
}

void SettingsSignaller::handleDesiredXRotationChanged(double val)
{
    this->desiredXRotation = (float)val;
}

void SettingsSignaller::handleHoldLengthChanged(int val)
{
    this->holdLength = val;
}