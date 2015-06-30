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
