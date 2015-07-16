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

#include "BaseMeasurements.h"

#include <Windows.h>

void BaseMeasurements::setBaseAngles(float roll, float pitch, float yaw)
{
	accessMutex.lock();
	baseRoll = roll;
	basePitch = pitch;
	baseYaw = yaw;
	accessMutex.unlock();
}

void BaseMeasurements::setBaseCursor(unsigned int x, unsigned int y)
{
	accessMutex.lock();
	baseCursorX = x;
	baseCursorY = y;
	accessMutex.unlock();
}

void BaseMeasurements::modifyBaseCursor(int xDelta, int yDelta)
{
    accessMutex.lock();

    if ((int)baseCursorX + xDelta < 0)
    {
        baseCursorX = 0;
    }
    else if ((int)baseCursorX + xDelta > screenSizeX)
    {
        baseCursorX = screenSizeX;
    }
    else
    {
        baseCursorX = (int)baseCursorX + xDelta;
    }
    if ((int)baseCursorY + yDelta < 0)
    {
        baseCursorY = 0;
    } 
    else if ((int)baseCursorY + yDelta > screenSizeY)
    {
        baseCursorY = screenSizeY;
    }
    else
    {
        baseCursorY = (int)baseCursorY + yDelta;
    }
    accessMutex.unlock();
}

// use Windows API to update cursor position
void BaseMeasurements::updateBaseCursor(int xyID)
{
	accessMutex.lock();
	POINT cursorPoint;
	if (GetCursorPos(&cursorPoint))
	{
		// success
        if (xyID <= 0)
        {
            baseCursorX = cursorPoint.x;
            baseCursorY = cursorPoint.y;
        }
        else if (xyID == X_ID)
        {
            baseCursorX = cursorPoint.x;
        }
        else if (xyID == Y_ID)
        {
            baseCursorY = cursorPoint.y;
        }
	}

	accessMutex.unlock();
}

void BaseMeasurements::updateScreenSize()
{
	accessMutex.lock();

	int xPixelsPrimScreen = GetSystemMetrics(SM_CXSCREEN);
	int yPixelsPrimScreen = GetSystemMetrics(SM_CYSCREEN);

	int xPixelsAllScreens = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int yPixelsAllScreens = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	//int x3 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	//int y3 = GetSystemMetrics(SM_YVIRTUALSCREEN);

    singleMonitorSizeX = xPixelsPrimScreen > 0 ? xPixelsPrimScreen : 1;
    singleMonitorSizeY = yPixelsPrimScreen > 0 ? yPixelsPrimScreen : 1;

    screenSizeX = xPixelsAllScreens > 0 ? xPixelsAllScreens : 1;
    screenSizeY = yPixelsAllScreens > 0 ? yPixelsAllScreens : 1;
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentAngles(float roll, float pitch, float yaw)
{
	accessMutex.lock();
	currentRoll = roll;
	currentPitch = pitch;
	currentYaw = yaw;
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentAnglesAsBase(int angleID)
{
	accessMutex.lock();
    if (angleID <= 0)
    {
        baseRoll = currentRoll;
        basePitch = currentPitch;
        baseYaw = currentYaw;
    }
    else if (angleID == ROLL_ID)
    {
        baseRoll = currentRoll;
    }
    else if (angleID == PITCH_ID)
    {
        basePitch = currentPitch;
    }
    else if (angleID == YAW_ID)
    {
        baseYaw = currentYaw;
    }
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentPose(myo::Pose pose)
{
	accessMutex.lock();
	currentPose = pose;
	accessMutex.unlock();
}

void BaseMeasurements::setCurrentState(midasMode state)
{
	accessMutex.lock();
	currentState = state;
	accessMutex.unlock();
}

bool BaseMeasurements::areCurrentValuesValid()
{
	accessMutex.lock();
	// check if current pose is caught up to myoState. if not, invalid.
	if (currentPose == (myoStateHandle->getPoseHistory()).back() && currentState == (controlStateHandle->getMode()))
	{
		accessMutex.unlock();
		return true;
	}
	accessMutex.unlock();
	return false;
}