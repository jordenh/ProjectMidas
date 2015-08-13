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

#include "MouseCtrl.h"
#include "BaseMeasurements.h"
#include "KeyboardVector.h"
#include "KeyboardContoller.h"
#include <iostream>
#include <time.h>

MouseCtrl::MouseCtrl()
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    lastMouseMoveX = clock() * (1000 / CLOCKS_PER_SEC);
    lastMouseMoveY = lastMouseMoveX;
    lastMouseScroll = lastMouseMoveX;
    minMoveXTimeDelta = DEFAULT_MIN_MOVE_TIME;
    minMoveYTimeDelta = DEFAULT_MIN_MOVE_TIME;
    scrollRate = WHEEL_DELTA;
    currHeld = 0;
    prevAbsMouseX = 0; prevAbsMouseY = 0;
    keyCodeModifier = -1;
}

MouseCtrl::~MouseCtrl()
{
}

void MouseCtrl::setScrollRate(int rate)
{
    if (rate < -WHEEL_DELTA || rate > WHEEL_DELTA)
    {
        scrollRate = WHEEL_DELTA;
    } 
    else
    {
        scrollRate = rate;
    }
}

void MouseCtrl::setMinMoveXTimeDelta(unsigned int rate)
{
    minMoveXTimeDelta = convertRateToDelta(rate);
}

void MouseCtrl::setMinMoveYTimeDelta(unsigned int rate)
{
    
    minMoveYTimeDelta = convertRateToDelta(rate);
}

unsigned int MouseCtrl::convertRateToDelta(unsigned int rate)
{
    if (rate < MOVE_RATE_DEADZONE)
    {
        return MAXUINT;
    }
    else if (rate < MOVE_RATE_DEADZONE + MOVE_RATE_SLOWZONE)
    {
        return MAX_MOVE_TIME_DELTA;
    }

    float ratePercent = min(1.0, max(((rate - MOVE_RATE_DEADZONE - MOVE_RATE_SLOWZONE) / (100.0 - MOVE_RATE_DEADZONE - MOVE_RATE_SLOWZONE)), 0));

    // calc min/max velocity in pixels/ms
    float maxVeloc = NUM_PIXEL_MOVE / (float)MIN_MOVE_TIME_DELTA;
    float minVeloc = NUM_PIXEL_MOVE / (float)MAX_MOVE_TIME_DELTA;
    float deltaVeloc = maxVeloc - minVeloc;

    float currVeloc = (deltaVeloc * ratePercent) + minVeloc;
    if (currVeloc <= 0.0)
    {
        // should not happen
        return MAXUINT;
    }

    return min(max(ceil(NUM_PIXEL_MOVE / currVeloc), MIN_MOVE_TIME_DELTA), MAX_MOVE_TIME_DELTA);
}

void MouseCtrl::sendCommand(mouseCmds mouseCmd, double mouseXRateIfMove, double mouseYRateIfMove)
{
    ZeroMemory(&mi, sizeof(MOUSEINPUT));
    DWORD currentTime = clock() * (1000 / CLOCKS_PER_SEC);
    DWORD deltaTimeXMove = currentTime - lastMouseMoveX;
    DWORD deltaTimeYMove = currentTime - lastMouseMoveY;
    DWORD deltaTimeScroll = currentTime - lastMouseScroll;

    setMouseInputVars(mouseCmd, mouseXRateIfMove, mouseYRateIfMove);

    // Handle Early exit cases if moving mouse
    if (mi.dwFlags == MOUSEEVENTF_MOVE 
        && 
        (((deltaTimeXMove < minMoveXTimeDelta && mi.dx != 0) ||
        (deltaTimeYMove < minMoveYTimeDelta && mi.dy != 0))
        || 
        (mouseXRateIfMove > 0 && mouseXRateIfMove < MOVE_RATE_DEADZONE)
        ||
        (mi.dx != 0 && mi.dy != 0 && mouseCmd != mouseCmds::MOVE_ABSOLUTE))
        )
    {
        // Not enough time has passed to move the mouse again or 
        // in deadzone or
        // movements not mutually exclusive.
        return;
    }
    else
    {
#ifdef JOYSTICK_CURSOR        
        // Update time stamps
        if (mi.dx != 0)
        {
            if (mouseXRateIfMove >= 0)
                setMinMoveXTimeDelta(mouseXRateIfMove); // July 17, 2015: I think  this is wrong, but not using joystick and dont remember purpose of this line so.. leaving in.
            lastMouseMoveX = currentTime;
        }
        if (mi.dy != 0)
        {
            if (mouseXRateIfMove >= 0)
                setMinMoveYTimeDelta(mouseXRateIfMove);
            lastMouseMoveY = currentTime;
        }
#else
        if (mi.dx != prevAbsMouseX)
        {
            lastMouseMoveX = currentTime;
            prevAbsMouseX = mi.dx;
        }
        if (mi.dy != prevAbsMouseY)
        {
            lastMouseMoveY = currentTime;
            prevAbsMouseY = mi.dy;
        }
#endif
    }

    // Handle early exit cases if scrolling mouse
    if (deltaTimeScroll < SCROLL_MIN_TIME &&
        (mi.dwFlags == MOUSEEVENTF_WHEEL ||
        mi.dwFlags == MOUSEEVENTF_HWHEEL))
    {
        // Not enough time has passed to move the mouse again
        return;
    } 
    else
    {
        lastMouseScroll = currentTime;
    }

    // Send Command!
    INPUT* in = new INPUT();
    in->type = INPUT_MOUSE;
    in->mi = mi;

    if (keyCodeModifier > 0)
    {
        sendModifierPressDown();
    }
    SendInput(1, in, sizeof(INPUT));
    if (keyCodeModifier > 0)
    {
        sendModifierRelease();
    }

    // Build and send opposite command if clicking!
    if (mouseCmd == mouseCmds::LEFT_CLICK ||
        mouseCmd == mouseCmds::RIGHT_CLICK ||
        mouseCmd == mouseCmds::MIDDLE_CLICK)
    {
        ZeroMemory(&mi, sizeof(MOUSEINPUT));
        switch (mouseCmd)
        {
        case mouseCmds::LEFT_CLICK:
            mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case mouseCmds::RIGHT_CLICK:
            mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case mouseCmds::MIDDLE_CLICK:
            mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            goto done;
        }

        Sleep(2);
        in->type = INPUT_MOUSE;
        in->mi = mi;

        if (keyCodeModifier > 0)
        {
            sendModifierPressDown();
        }
        SendInput(1, in, sizeof(INPUT));
        if (keyCodeModifier > 0)
        {
            sendModifierRelease();
        }
    }

    keyCodeModifier = -1;

    done:
    delete in; in = NULL;
}

void MouseCtrl::setMouseInputVars(mouseCmds mouseCmd, double& mouseXRateIfMove, double& mouseYRateIfMove)
{
    keyCodeModifier = -1;

    if (mouseCmd == MOVE_HOR && mouseXRateIfMove < 0)
    {
        mouseCmd = MOVE_LEFT;
    }
    else if (mouseCmd == MOVE_HOR && mouseXRateIfMove >= 0)
    {
        mouseCmd = MOVE_RIGHT;
    }
    else if (mouseCmd == MOVE_VERT && mouseXRateIfMove < 0)
    {
        mouseCmd = MOVE_DOWN;
    }
    else if (mouseCmd == MOVE_VERT && mouseXRateIfMove >= 0)
    {
        mouseCmd = MOVE_UP;
    }

	if (mouseCmd != mouseCmds::MOVE_ABSOLUTE)
	{
        mouseXRateIfMove = abs(mouseXRateIfMove);
	}

    switch (mouseCmd)
    {
    case mouseCmds::LEFT_HOLD:
        currHeld |= LEFT_HELD;
    case mouseCmds::LEFT_CLICK:
        mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case mouseCmds::RIGHT_HOLD:
        currHeld |= RIGHT_HELD;
    case mouseCmds::RIGHT_CLICK:
        mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case mouseCmds::MIDDLE_HOLD:
        currHeld |= MID_HELD;
    case mouseCmds::MIDDLE_CLICK:
        mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    case mouseCmds::LEFT_RELEASE:
        currHeld &= !LEFT_HELD;
        mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case mouseCmds::RIGHT_RELEASE:
        currHeld &= !RIGHT_HELD;
        mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    case mouseCmds::MIDDLE_RELEASE:
        currHeld &= !MID_HELD;
        mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    case mouseCmds::RELEASE_LRM_BUTS:
        mi.dwFlags = 0;
        if ((currHeld & LEFT_HELD) != 0)
        {
            currHeld &= !LEFT_HELD;
            mi.dwFlags |= MOUSEEVENTF_LEFTUP;
        }
        if ((currHeld & RIGHT_HELD) != 0)
        {
            currHeld &= !RIGHT_HELD;
            mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
        }
        if ((currHeld & MID_HELD) != 0)
        {
            currHeld &= !MID_HELD;
            mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
        }
        break;
    case mouseCmds::MOVE_LEFT:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = -1;
        break;
    case mouseCmds::MOVE_RIGHT:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dx = 1;
        break;
    case mouseCmds::MOVE_UP:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = -1; // negative moves up.
        break;
    case mouseCmds::MOVE_DOWN:
        mi.dwFlags = MOUSEEVENTF_MOVE;
        mi.dy = 1;
        break;
    case mouseCmds::SCROLL_LEFT:
        // Doesnt work without elevated privelages...
        mi.dwFlags = MOUSEEVENTF_HWHEEL;
        mi.mouseData = -scrollRate;
        break;
    case mouseCmds::SCROLL_RIGHT:
        // Doesnt work without elevated privelages...
        mi.dwFlags = MOUSEEVENTF_HWHEEL;
        mi.mouseData = scrollRate;
        break;
    case mouseCmds::SCROLL_UP:
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = abs(scrollRate); // RANGE IS FROM -120 to +120 : WHEEL_DELTA = 120, which is one "wheel click"
        break;
    case mouseCmds::SCROLL_DOWN:
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = -abs(scrollRate); // RANGE IS FROM -120 to +120 : WHEEL_DELTA = 120, which is one "wheel click"
        break;
    case mouseCmds::SHIFT_SCROLL_UP:
        keyCodeModifier = VK_LSHIFT;
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = abs(scrollRate); // RANGE IS FROM -120 to +120 : WHEEL_DELTA = 120, which is one "wheel click"
        break;
    case mouseCmds::SHIFT_SCROLL_DOWN:
        keyCodeModifier = VK_LSHIFT;
        mi.dwFlags = MOUSEEVENTF_WHEEL;
        mi.mouseData = -abs(scrollRate); // RANGE IS FROM -120 to +120 : WHEEL_DELTA = 120, which is one "wheel click"
        break;
	case mouseCmds::MOVE_ABSOLUTE:
		if (!BaseMeasurements::getInstance().areCurrentValuesValid())
		{
			break;
		}

		mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		int monitorSizeWeight = 65535; // size of a single monitor as represented by windows API
        float monitorWidth = BaseMeasurements::getInstance().getSingleMonitorSizeX();
        float monitorHeight = BaseMeasurements::getInstance().getSingleMonitorSizeY();

		float baseCursorX = BaseMeasurements::getInstance().getBaseCursorX();
        float baseCursorY = BaseMeasurements::getInstance().getBaseCursorY();
        float baseWindowsLocX = (baseCursorX / monitorWidth) * monitorSizeWeight;
        float baseWindowsLocY = (baseCursorY / monitorHeight) * monitorSizeWeight;

        mi.dx = baseWindowsLocX + (mouseXRateIfMove / 100.0 * monitorSizeWeight / 2);
        mi.dy = baseWindowsLocY + (mouseYRateIfMove / 100.0 * monitorSizeWeight / 2);

        int maxDx = monitorSizeWeight * (BaseMeasurements::getInstance().getScreenSizeX() / monitorWidth);
        int maxDy = monitorSizeWeight * (BaseMeasurements::getInstance().getScreenSizeY() / monitorHeight);
        mi.dx = max(min(mi.dx, maxDx), 0);
        mi.dy = max(min(mi.dy, maxDy), 0);

        if (mi.dx <= 0 || 
            mi.dx >= maxDx)
        {
            BaseMeasurements::getInstance().setCurrentAnglesAsBase(YAW_ID);
            BaseMeasurements::getInstance().updateBaseCursor(X_ID);
        }
        if (mi.dy <= 0 ||
            mi.dy >= maxDy)
        {
            BaseMeasurements::getInstance().setCurrentAnglesAsBase(PITCH_ID);
            BaseMeasurements::getInstance().updateBaseCursor(Y_ID);
        }

		break;
    }
}

void MouseCtrl::sendModifierPressDown()
{
    KeyboardVector kv;
    kv.inputVKDown(keyCodeModifier);
    KeyboardController kc;
    kc.setKiVector(kv);
    kc.sendData();
}

void MouseCtrl::sendModifierRelease()
{
    KeyboardVector kv;
    kv.inputVKUp(keyCodeModifier);
    KeyboardController kc;
    kc.setKiVector(kv);
    kc.sendData();
}