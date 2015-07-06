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

/* Singleton */

class BaseMeasurements {
public:
	static BaseMeasurements& getInstance()
	{
		static BaseMeasurements instance;
		return instance;
	}

	void setBaseAngles(float roll, float pitch, float yaw);

	void setBaseCursor(unsigned int x, unsigned int y);

	// use Windows API to update cursor position
	void updateBaseCursor();

	void setScreenSize(unsigned int x, unsigned int y);

	void setCurrentAngles(float roll, float pitch, float yaw);

	void setCurrentAnglesAsBase();

	void setCurrentPose(myo::Pose pose);
	
	void setCurrentState(midasMode state);

	void setMyoStateHandle(MyoState* myoState) { myoStateHandle = myoState; }

	void setControlStateHandle(ControlState* controlState) { controlStateHandle = controlState; }

	float getBaseRoll() { return baseRoll; }
	float getBasePitch() { return basePitch; }
	float getBaseYaw() { return baseYaw; }
	float getBaseCursorX() { return baseCursorX; }
	float getBaseCursorY() { return baseCursorY; }
	float getScreenSizeX() { return screenSizeX; }
	float getScreenSizeY() { return screenSizeY; }
	myo::Pose BaseMeasurements::getCurrentPose() { return currentPose; }
	midasMode BaseMeasurements::getCurrentState() { return currentState; }

	bool areCurrentValuesValid();

private:
	// Force as singleton
	BaseMeasurements() { myoStateHandle = NULL; currentState = LOCK_MODE; currentPose = myo::Pose::rest; };
	//~BaseMeasurements();
	BaseMeasurements(BaseMeasurements const&) = delete;
	void operator=(BaseMeasurements const&) = delete;

	std::mutex accessMutex;

	float baseRoll;
	float basePitch;
	float baseYaw;

	unsigned int baseCursorX;
	unsigned int baseCursorY;

	unsigned int screenSizeX;
	unsigned int screenSizeY;

	// helper info
	float currentRoll;
	float currentPitch;
	float currentYaw;

	midasMode currentState;
	myo::Pose currentPose;

	MyoState* myoStateHandle; // not owned
    ControlState* controlStateHandle; // not owned
};

#endif /* BASE_MEASUREMENTS_H */