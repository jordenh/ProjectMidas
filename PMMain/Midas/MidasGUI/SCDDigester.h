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

#include "RingData.h"

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

/* 
 * Used to continually poll the Shared Command Data and pass the command data 
 * to handling functions
 */
class SCDDigester
{
public:
    /*
     * Basic Constructor/Destructor
     */
	SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MyoState *myoStateHandle,
        MouseCtrl *mouseCtrl, KeyboardController *keyboardController, ProfileManager* profileManagerHandle, MainGUI *mainGui, std::vector<ringData> *kybrdRingData);

    ~SCDDigester();

    /*
     * Main function, which takes data from the global Shared Command Data
     * and 'digests' it by passing it to the appropriate handlers/dealing with
     * it itself
     */
    void digest();

private:
    /*
     * Passes commands to appropriate handlers
     *
     * @param nextCommand The command to be executed
     */
    void digestKybdCmd(CommandData nextCommand);

    /*
     * Executes all required functions to change a profile through a dynamic command
     *
     * @param nextCmd containing the profile change information
     */
	void digestProfileChange(CommandData nextCmd);

    /*
     * Handles the change of the Wearable Device's connected state
     */
    void handleConnectionData();

    /*
     * Passes commands to appropriate keyboard handlers
     *
     * @param nextCommand containing the keyboard GUI command
     */
    void digestKeyboardGUIData(CommandData nextCommand);

    /*
     * Gets the key index from a given angle and ring
     *
     * @param angle The current angle of the user's arm
     * @param ring The currently active ring, used to determine angle division (360/ring->size) per division
     * @return int the index of the selected key in ring
     */
	int getSelectedKeyFromAngle(double angle, std::vector<ringData::keyboardValue> *ring);

	std::vector<ringData> *kybrdRingData; // not owned
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

    int prevMyoBatLevel;
    int prevMyoRssi;

    ProfileManager *pm; // not owned
};

#endif /* _SCD_DIGESTER_H */