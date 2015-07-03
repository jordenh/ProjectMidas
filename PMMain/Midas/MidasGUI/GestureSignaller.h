#ifndef GESTURE_SIGNALLER_H
#define GESTURE_SIGNALLER_H

#include "QObject.h"
#include "MyoCommon.h"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#else
#include "myo\myo.hpp"
#endif

/**
 * The GestureSignaller class handles the communication between the Midas sequence logic
 * and the SequenceDisplayer GUI.
 */
class GestureSignaller : public QObject
{
    Q_OBJECT

public:
    GestureSignaller();
    ~GestureSignaller(); 

    bool getShowAll();

public slots:
    void handleShowAllToggle(bool showAll);

signals:
    void emitRegisterSequence(int, QString, std::vector<sequenceImageSet>);
    void emitShowSequences(std::vector<sequenceProgressData>);

    void emitStateString(QString);

    void emitPoseImages(std::vector<sequenceImageSet>);

    void emitToggleKeyboard();

    // true if change forwared, false if backwards
    void emitProfileChange(bool);
private:
    static bool showAll;
};

#endif GESTURE_SIGNALLER_H