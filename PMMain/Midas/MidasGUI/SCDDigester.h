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

#ifndef _SCD_DIGESTER_H
#define _SCD_DIGESTER_H

#ifdef BUILD_KEYBOARD
#include "RingData.h"
#endif

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
#include "myo\myo.hpp"
using namespace myo;
#endif

class ConnectionSignaller;
class SettingsSignaller;
class CommandData;
class KeyboardController;
class SharedCommandData;
class MidasThread;
class ControlState;
class MyoState;
class MouseCtrl;
class ProfileManager;
class MainGUI;

class SCDDigester
{
public:
#ifdef BUILD_KEYBOARD
	SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MyoState *myoStateHandle,
        MouseCtrl *mouseCtrl, KeyboardController *keyboardController, ProfileManager* profileManagerHandle, MainGUI *mainGui, std::vector<ringData> *kybrdRingData);
#else
	SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MyoState *myoStateHandle,
        MouseCtrl *mouseCtrl, KeyboardController *keyboardController, ProfileManager* profileManagerHandle, MainGUI *mainGui);
#endif
    ~SCDDigester();

    void digest();

private:
    void digestKybdCmd(CommandData nextCommand);

	void digestProfileChange(CommandData nextCmd);

    void handleConnectionData();

#ifdef BUILD_KEYBOARD
    void digestKeyboardGUIData(CommandData nextCommand);

	int getSelectedKeyFromAngle(double angle, std::vector<ringData::keyboardValue> *ring);

	std::vector<ringData> *kybrdRingData;
#endif

	KeyboardController *keyboardController; // not owned
    SharedCommandData *scdHandle; // not owned
    MidasThread *threadHandle; // not owned
    ControlState *cntrlStateHandle; // not owned
    MyoState *myoStateHandle; // not owned
    MouseCtrl *mouseCtrl; // not owned
    MainGUI *mainGUI; // not owned
    int count;

    ConnectionSignaller *connSignaller; // owned
    static SettingsSignaller settingsSignaller;

    ProfileManager *pm; // not owned
};

#endif /* _SCD_DIGESTER_H */