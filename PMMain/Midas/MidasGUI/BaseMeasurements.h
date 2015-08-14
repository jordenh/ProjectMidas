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

#ifndef BASE_MEASUREMENTS_H
#define BASE_MEASUREMENTS_H

#include "myo\myo.hpp"
#include "MyoState.h"
#include "ControlState.h"
#include <Windows.h>

#define ROLL_ID 1
#define PITCH_ID 2
#define YAW_ID 3
#define X_ID 1
#define Y_ID 2

/* Singleton */

class BaseMeasurements {
public:
	static BaseMeasurements& getInstance()
	{
		static BaseMeasurements instance;
		return instance;
	}

	void setBaseAngles(float roll, float pitch, float yaw);

    void setInvalid(bool invalid);

	void setBaseCursor(unsigned int x, unsigned int y);

    void modifyBaseCursor(int xDelta, int yDelta);

	// use Windows API to update cursor position
	void updateBaseCursor(int xyID = 0);

	void updateScreenSize();

	void setCurrentAngles(float roll, float pitch, float yaw);

    // <= 0 for all angles, ROLL_ID for just roll, PITCH_ID for pitch, YAW_ID for yaw
	void setCurrentAnglesAsBase(int angleID = 0);

	void setCurrentPose(myo::Pose pose);
	
	void setCurrentState(midasMode state);

	void setMyoStateHandle(MyoState* myoState) { myoStateHandle = myoState; }

	void setControlStateHandle(ControlState* controlState) { controlStateHandle = controlState; }

    float getCurrentRoll()    { accessMutex.lock();   float retVal = currentRoll;     accessMutex.unlock(); return retVal; }
    float getCurrentPitch()   { accessMutex.lock();   float retVal = currentPitch;    accessMutex.unlock(); return retVal; }
    float getCurrentYaw()     { accessMutex.lock();   float retVal = currentYaw;      accessMutex.unlock(); return retVal; }
	float getBaseRoll()    { accessMutex.lock();   float retVal = baseRoll;     accessMutex.unlock(); return retVal; }
	float getBasePitch()   { accessMutex.lock();   float retVal = basePitch;    accessMutex.unlock(); return retVal; }
	float getBaseYaw()     { accessMutex.lock();   float retVal = baseYaw;      accessMutex.unlock(); return retVal; }
    bool  getInvalid()     { accessMutex.lock();   bool retVal = invalid;      accessMutex.unlock(); return retVal; }
	float getBaseCursorX() { accessMutex.lock();   float retVal = baseCursorX;  accessMutex.unlock(); return retVal; }
	float getBaseCursorY() { accessMutex.lock();   float retVal = baseCursorY;  accessMutex.unlock(); return retVal; }
    static POINT getCurrentCursor();
	float getScreenSizeX() { accessMutex.lock();   float retVal = screenSizeX;  accessMutex.unlock(); return retVal; }
	float getScreenSizeY() { accessMutex.lock();   float retVal = screenSizeY;  accessMutex.unlock(); return retVal; }
    float getSingleMonitorSizeX() { accessMutex.lock();   float retVal = singleMonitorSizeX;  accessMutex.unlock(); return retVal; }
    float getSingleMonitorSizeY() { accessMutex.lock();   float retVal = singleMonitorSizeY;  accessMutex.unlock(); return retVal; }
    myo::Pose BaseMeasurements::getCurrentPose() { accessMutex.lock(); myo::Pose retVal = currentPose;  accessMutex.unlock(); return retVal; }
    midasMode BaseMeasurements::getCurrentState() { accessMutex.lock(); midasMode retVal = currentState; accessMutex.unlock(); return retVal; }

	bool areCurrentValuesValid();

private:
	// Force as singleton
	BaseMeasurements() { 
        invalid = false;
        baseRoll = 0; basePitch = 0; baseYaw = 0;
        baseCursorX = 0; baseCursorY = 0; 
        updateScreenSize();
        currentRoll = 0; currentPitch = 0; currentYaw = 0;
        myoStateHandle = NULL;
        controlStateHandle = NULL;
        currentState = LOCK_MODE;
        currentPose = myo::Pose::rest; 
    };
	//~BaseMeasurements();
	BaseMeasurements(BaseMeasurements const&) = delete;
	void operator=(BaseMeasurements const&) = delete;

	std::mutex accessMutex;

	float baseRoll;
	float basePitch;
	float baseYaw;

	unsigned int baseCursorX;
	unsigned int baseCursorY;

    // ASSUMPTION : ALL MONITORS ARE THE SAME AS THE PRIMARY MONITOR. If not true, Midas will not work properly
    unsigned int singleMonitorSizeX;
    unsigned int singleMonitorSizeY;
	unsigned int screenSizeX;
	unsigned int screenSizeY;

	// helper info
	float currentRoll;
	float currentPitch;
	float currentYaw;

    // value used to overide validation, which can be externally set/unset.
    bool invalid;

	midasMode currentState;
	myo::Pose currentPose;

	MyoState* myoStateHandle; // not owned
    ControlState* controlStateHandle; // not owned
};

#endif /* BASE_MEASUREMENTS_H */