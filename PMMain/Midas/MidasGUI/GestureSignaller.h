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