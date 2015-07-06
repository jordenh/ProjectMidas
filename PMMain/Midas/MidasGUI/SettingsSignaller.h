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

#ifndef SETTINGS_SIGNALLER_H
#define SETTINGS_SIGNALLER_H

#include <QObject>
#include "MidasCommon.h"
#include "MyoCommon.h"

class SettingsSignaller : public QObject
{
    Q_OBJECT

public:
    SettingsSignaller(QObject *parent = 0, unsigned int yawMaxAngle = INIT_YAW_ANGLE, unsigned int pitchMaxAngle = INIT_PITCH_ANGLE, buzzFeedbackMode buzzFeedback = MINIMAL);
    ~SettingsSignaller();

    unsigned int getYawMaxAngle() { return yawMaxAngle; }
    unsigned int getPitchMaxAngle() { return pitchMaxAngle; }
    buzzFeedbackMode getBuzzFeedbackMode() { return buzzFeedback; }
private:
    unsigned int yawMaxAngle;
    unsigned int pitchMaxAngle;

    buzzFeedbackMode buzzFeedback;

public slots:
    void handleSliderValues(unsigned int yawMaxAngle, unsigned int pitchMaxAngle);

    void handleBuzzFeedbackChange(unsigned int buzzMode);
};

#endif // SETTINGS_SIGNALLER_H
