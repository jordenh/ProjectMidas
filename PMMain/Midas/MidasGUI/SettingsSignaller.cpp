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