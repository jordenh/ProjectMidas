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

SettingsSignaller::SettingsSignaller(QObject *parent, unsigned int yawMaxAngle, unsigned int pitchMaxAngle, buzzFeedbackMode buzzFeedback)
    : QObject(parent), yawMaxAngle(yawMaxAngle), pitchMaxAngle(pitchMaxAngle), buzzFeedback(buzzFeedback)
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