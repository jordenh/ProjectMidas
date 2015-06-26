#ifndef SETTINGS_SIGNALLER_H
#define SETTINGS_SIGNALLER_H

#include <QObject>
#include "MidasCommon.h"

class SettingsSignaller : public QObject
{
    Q_OBJECT

public:
    SettingsSignaller(QObject *parent = 0, unsigned int yawMaxAngle = INIT_YAW_ANGLE, unsigned int pitchMaxAngle = INIT_PITCH_ANGLE);
    ~SettingsSignaller();

    unsigned int getYawMaxAngle() { return yawMaxAngle; }
    unsigned int getPitchMaxAngle() { return pitchMaxAngle; }

private:
    unsigned int yawMaxAngle;
    unsigned int pitchMaxAngle;

public slots:
    void handleSliderValues(unsigned int yawMaxAngle, unsigned int pitchMaxAngle);
};

#endif // SETTINGS_SIGNALLER_H
