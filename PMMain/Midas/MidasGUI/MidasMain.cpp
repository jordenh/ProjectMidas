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

#include "MidasMain.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <thread>

#include "MidasThread.h"
#include "ProfileManager.h"
#include "SCDDigester.h"
#include "MouseCtrl.h"
#include "WearableDevice.h"
#include "SharedCommandData.h"
#include "MyoDevice.h"
#include "KeyboardContoller.h"
#include "BaseMeasurements.h"
#include "MidasThread.h"
#include "SharedCommandDataTest.h"
#include "MouseCtrlTest.h"
#include "MyoState.h"
#include "KeyboardWidget.h"
#include "ringData.h"
#include "KeyboardSettingsReader.h"

using namespace std;

#define MAIN_MODE

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
#endif

#ifdef USE_SIMULATOR
using namespace myoSim;
#else
using namespace myo;
#endif

int midasMain(MidasThread *threadHandle, MainGUI *mainGui, ProfileManager *pm, std::vector<ringData> *kybrdRingData) {

    std::cout << "starting Midas Main" << std::endl;

#ifdef TEST_WEARABLE_DEVICE
    SharedCommandDataTest::testQueue();
#endif

#ifdef MOUSE_CONTROL_TEST
    cout << "Starting Move Mouse Square tests" << endl;

    Sleep(3000);

    MouseCtrlTest::testMouseSquare(25); // slower
    MouseCtrlTest::testMouseSquare(50); // med
    MouseCtrlTest::testMouseSquare(100); // fast
    
    cout << "Starting Scroll Mouse Square (up/down for now) tests" << endl;
    Sleep(1000);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, 4); //moderately slow 4/120 speed, and 5 ticks total.
    Sleep(500);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, 10); //moderately slow 10/120 speed, and 5 ticks total.
    Sleep(500);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, 40); // faster 40/120 speed, and 5 ticks total.
    Sleep(500);
    MouseCtrlTest::testScrollMouseSquare(WHEEL_DELTA * 5, WHEEL_DELTA); // fastest 120/120 speed, and 5 ticks total.
    
    cout << "Starting Scroll Zoom Tests" << endl;
    Sleep(1000);
    MouseCtrlTest::testScrollZoomMouse(WHEEL_DELTA * 5, 4);
    Sleep(500);
    MouseCtrlTest::testScrollZoomMouse(WHEEL_DELTA * 5, 10);
    Sleep(500);
    MouseCtrlTest::testScrollZoomMouse(WHEEL_DELTA * 5, 20);

#endif

#ifdef MAIN_MODE
	SharedCommandData sharedData(kybrdRingData->size() * 2);

    ControlState controlState(&sharedData);
	MyoState myoState;
	myoState.setPoseHistLen(5); // arbitrary for now.
	myoState.setSpatialHistLen(5);
	BaseMeasurements::getInstance().setMyoStateHandle(&myoState);
	BaseMeasurements::getInstance().setControlStateHandle(&controlState);
	MyoDevice* myoDevice = new MyoDevice(&sharedData, &controlState, &myoState, "com.midas.midas-test", mainGui, pm);
    myoState.setMyo(myoDevice);
    MouseCtrl* mouseCtrl = new MouseCtrl();
	KeyboardController* keyboardController = new KeyboardController();

    // Kick off device thread
    startWearableDeviceListener(myoDevice); // TODO - add a flag in myoDevice to see if it is running. Don't enter 'while true' until running.

    SCDDigester scdDigester(&sharedData, threadHandle, &controlState, &myoState, mouseCtrl, keyboardController, pm, mainGui, kybrdRingData);

    while (true)
    {
        // Improve performance by a tremendous amount by sleeping in this loop for a short
        // period of time (ie 15% CPU down to 2% on an i7).
       Sleep(2);

       if (myoDevice->getDeviceStatus() != deviceStatus::RUNNING) 
           break;

       scdDigester.digest();
    }

	delete myoDevice;
	delete mouseCtrl;
	delete keyboardController;

#endif

    exit(0);
    return 0;
}