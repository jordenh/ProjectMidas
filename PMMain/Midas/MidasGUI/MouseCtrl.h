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

#ifndef _MOUSE_CONTROLLER_H
#define _MOUSE_CONTROLLER_H


#include <Windows.h>
#include <vector>
#include "MidasCommon.h"

#define MONITOR_SIZE_WEIGHT 65535.0 // size of a single monitor as represented by windows API

#define DEFAULT_MIN_MOVE_TIME 10
#define SCROLL_MIN_TIME 5
#define MAX_MOVE_TIME_DELTA 40 //large enough ms delay between moving a pixel is pretty slow.
#define MIN_MOVE_TIME_DELTA 1 //small enough ms delay between moving a pixel is fast, but not uncontrolled..
#define MOVE_RATE_DEADZONE 20
#define MOVE_RATE_SLOWZONE 20
#define NUM_PIXEL_MOVE 1

#define LEFT_HELD 0x1
#define RIGHT_HELD 0x2
#define MID_HELD 0x4

/**
 * Handles sending mouse data to Windows.
 */
class MouseCtrl
{
public:
    /* Basic Construction/Destruction */
    MouseCtrl();
    ~MouseCtrl();

    /**
     * Sets the scroll rate to control speed and direction.
     *
     * @param rate The rate of scrolling, between -120 and 120. A negative
     * value represents a downward scroll, positive is upward.
     */
    void setScrollRate(int rate);

    /**
     * Sets the rate at which the cursor should move in the X axis.
     *
     * @param rate The rate of cursor movement in the X axis, between
     * 0 and 100.
     */
    void setMinMoveXTimeDelta(unsigned int rate);

    /**
     * Sets the rate at which the cursor should move in the Y axis.
     *
     * @param rate The rate of cursor movement in the Y axis, between 0 and 100.
     */
    void setMinMoveYTimeDelta(unsigned int rate);

    /**
    *  Abstracts function for linearizing rate into velocity
    *
    * @param rate The rate to convert into a velocity (from 0 to 100)
    */
    unsigned int convertRateToDelta(unsigned int rate);

    /**
     * Sends a mouse command to the OS. This includes movement commands. The mouseRateIfMove parameter
     * sets the rate of the mouse movement if it is nonnegative. 
     *
     * @param mouseCmd The mouse command to send.
     * @param mouseRateIfMove The new X positional fraction of the cursor
     * @param mouseYRateIfMove The new Y positional fraction of the cursor
     */
    void sendCommand(mouseCmds mouseCmd, double mouseRateIfMove = 0.0, double mouseYRateIfMove = 0.0);

    unsigned int getCurrentlyHeldButton() { return currHeld; }

    void notifyControllerOfEnteringMouseMode();
    void notifyControllerOfLeavingMouseMode();

private:

    /**
     * Sets the fields of the MOUSEINPUT that will be sent to Windows.
     * @param mouseCmd The mouse command to be executed
     * @param mouseRateIfMove The rate from -100 to 100 a mouse should move. The new X positional fraction of the cursor
     * @param mouseYRateIfMove The rate from -100 to 100 a mouse should move. The new Y positional fraction of the cursor
     * This determines velocity. This value gets normalized to absolute value.
     */
    void setMouseInputVars(mouseCmds mouseCmd, double& mouseRateIfMove, double& mouseYRateIfMove);

    /**
     * Presses Down the VK_ key associated with keyCodeModifier. This is intended to be
     * done right before the mouse command is sent, if the keyCodeModifier is positive.
     */
    void sendModifierPressDown();

    /**
     * Release the VK_ key associated with keyCodeModifier. This is intended to be
     * done right after the mouse command is sent, if the keyCodeModifier is positive.
     */
    void sendModifierRelease();

    // helper functions to translate between the location in pixels on a screen, to/from the location
    // in windows frame coordinates (represented as MONITOR_SIZE_WEIGHT per screen width/height).
    // For example, on a 1920x1080 monitor, location 960,540 would correspond to MONITOR_SIZE_WEIGHT/2, MONITOR_SIZE_WEIGHT/2
    float pixelXLocToWindowsXLoc(float pixelLoc);
    float pixelYLocToWindowsYLoc(float pixelLoc);
    float windowsXLocToPixelXLoc(float windowsLoc);
    float windowsYLocToPixelYLoc(float windowsLoc);
    float monitorWidth;
    float monitorHeight;

    // Mouse Input 
    MOUSEINPUT mi;

    // Time Stamps used to limit move and scroll speed.
    DWORD lastMouseMoveX;
    DWORD lastMouseMoveY;
    DWORD lastMouseScroll;

    // Min time that must pass until x/y of cursor can move one pixel.
    // This is dictated by moveX/YRate, as rates are easier to conceptualize
    // for caller.
    DWORD minMoveXTimeDelta;
    DWORD minMoveYTimeDelta;

    // value from -WHEEL_DELTA to WHEEL_DELTA dictating portion of a scroll click
    // will happen when the "scrollwheel is moved"
    int scrollRate;

    // range from 0 to 4:
    // 0 = none held.
    // 1 = left held.
    // 2 = right held.
    // 4 = middle held.
    unsigned char currHeld;

    bool inMouseMode;

    int prevAbsMouseX;
    int prevAbsMouseY;

    // Add these modifiers to any absolute cursor placement actions to 
    // account for other mice in the system.
    int otherMiceXModifier;
    int otherMiceYModifier;

    // A keycode to hold down when the mouse action is performed(some applications change affect if shift is held on scroll, etc).
    int keyCodeModifier;
};

#endif /* _MOUSE_CONTROLLER_H */