#include "SettingsSignaller.h"

SettingsSignaller::SettingsSignaller(QObject *parent, unsigned int yawMaxAngle, unsigned int pitchMaxAngle)
    : QObject(parent), yawMaxAngle(yawMaxAngle), pitchMaxAngle(pitchMaxAngle)
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
