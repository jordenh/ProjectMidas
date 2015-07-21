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

#ifndef _MIDAS_COMMON_H
#define _MIDAS_COMMON_H

#include <string>
#include <map>

#ifdef USE_GYRO_DATA
#define INIT_PITCH_ANGLE 100.0f /* Maximum delta angle in degrees */
#define INIT_YAW_ANGLE 100.0f /* Maximum delta angle in degrees */
#else
#define INIT_PITCH_ANGLE 20.0f /* Maximum delta angle in degrees */
#define INIT_YAW_ANGLE 25.0f /* Maximum delta angle in degrees */
#endif
#define MIN_SLIDER_ANGLE 15
#define MAX_SLIDER_ANGLE 100

#define MIN_GYRO_POW 1
#define MAX_GYRO_POW 100
#define DEFAULT_GYRO_POW 2

#define MIN_GYRO_SCALE_DOWN 10
#define MAX_GYRO_SCALE_DOWN 100000
#define DEFAULT_GYRO_SCALE_DOWN 100

#define GUI_OPACITY 0.70

#ifdef SHOW_PROFILE_ICONS
#define GRID_ELEMENT_SIZE 48
#define MOUSE_INDICATOR_SIZE 130
#define INFO_INDICATOR_WIDTH   150
#else
#define GRID_ELEMENT_SIZE   52
#define MOUSE_INDICATOR_SIZE 109 // this is 2 * GRID_ELEMENT_SIZE + WIDGET_BUFFER
#define INFO_INDICATOR_WIDTH   109
#endif
#define INFO_INDICATOR_HEIGHT  35
#define PROF_INDICATOR_WIDTH   250
#define PROF_INDICATOR_HEIGHT  35
#define SETTINGS_HEIGHT 170
#define SETTINGS_WIDTH 400
#define WIDGET_BUFFER 5
#define DISTANCE_DISPLAY_WIDTH 250
#define DISTANCE_DISPLAY_HEIGHT 40
#define GUI_HEIGHT_OFFSET_FROM_BOTTOM (MOUSE_INDICATOR_SIZE + INFO_INDICATOR_HEIGHT + 3*WIDGET_BUFFER)

#define SPECIFIC_PROFILE_ICON_SIZE 60
#define PROFILE_ICON0_ACTIVE	"Resources\\icons\\Ring_Post2.png"	
#define PROFILE_ICON0_INACTIVE	"Resources\\icons\\RingBW2.png"		
#define PROFILE_ICON1_ACTIVE	"Resources\\icons\\Pencil_Post.png"
#define PROFILE_ICON1_INACTIVE	"Resources\\icons\\PencilBW.png"

#define MIDAS_GREY QColor(205, 205, 193)
#define MYO_BLUE QColor(0, 188, 223)
#define KEYBOARD_SEL QColor(200, 0, 0)

class KeyboardVector;

/**
 * Enumerates the types of commands that can be sent to control
 * peripherals on Windows.
 */
enum commandType {
    KYBRD_CMD,
    KYBRD_GUI_CMD,
    MOUSE_CMD,
    STATE_CHANGE,
	PROFILE_CHANGE,
    NONE,
    UNKNOWN_COMMAND
};

enum profileCmds {
	MOVE_PROFILE_FORWARD,
	MOVE_PROFILE_BACKWARD
};

/**
*  Keyboard GUI commands, used to perform actions across the 
*  Midas main/GUI threads.
*/
enum kybdGUICmds {
    SWAP_RING_FOCUS,
    CHANGE_WHEELS,
    SELECT,
    HOLD_SELECT
};

/**
 * High-level keyboard commands that represent common actions.
 */
enum kybdCmds {
    NO_CMD = 0,
    UNDO = 1,
    REDO = 2,
    ZOOM_IN = 4,
    ZOOM_OUT = 8,
    ZOOM_100 = 16,
    ESCAPE = 32,
    ENTER = 64,
    TAB = 128,
    SWITCH_WIN_FORWARD = 256,
    SWITCH_WIN_REVERSE = 512,
    COPY = 1024,
    PASTE = 2048,
    CUT = 4096,
    FILE_MENU = 8192,
    NEW_BROWSER = 16384,
    GOTO_ADDR_BAR = 32768,
    LOCK_DESKTOP = 65536,
    EDIT_MENU = 131072,
    VIEW_MENU = 262144,
    WIN_HOME = 524288,
    HIDE_APPS = 1048576,
    CONTROL = 2097152,
    VOLUME_UP = 4194304,
    VOLUME_DOWN = 8388608,
    BACKSPACE = 16777216,
    UP_ARROW = 33554432,
    RIGHT_ARROW = 67108864,
    DOWN_ARROW = 134217728,
    LEFT_ARROW = 268435456,
	INPUT_VECTOR = 536870912 // 2^29 ... NEXT = 1073741824
};

/**
 * Mouse commands caused by buttons, wheels and other features
 * that are a part of a mouse.
 */
enum mouseCmds {
    LEFT_CLICK,
    RIGHT_CLICK,
    MIDDLE_CLICK,
    LEFT_HOLD,
    RIGHT_HOLD,
    MIDDLE_HOLD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_HOR,
    MOVE_VERT,
    SCROLL_LEFT,
    SCROLL_RIGHT,
    SCROLL_UP,
    SCROLL_DOWN,
    LEFT_RELEASE,
    RIGHT_RELEASE,
    MIDDLE_RELEASE,
    RELEASE_LRM_BUTS,
	MOVE_ABSOLUTE
};

/**
 * This represents the state/mode of the application. Each mode has
 * different rules that must be enforced by each Midas device.
 *
 * NOTE: IF THIS IS CHANGED: any code that follows a similar format to
 * (int midasModeInt = midasMode::LOCK_MODE; midasModeInt <= midasMode::GESTURE_HOLD_FIVE; midasModeInt++)
 * NEEDS to be updated, or else errors can occur. For example, GestureSeqRecorder.cpp uses this type
 * of code to loop through the midasModes.
 */
enum midasMode {
    LOCK_MODE,      /**< The mouse is locked from movement. */
    MOUSE_MODE,     /**< The mouse is free to move around. */
    MOUSE_MODE2, /**< The mouse is free to move around, but with different configurations */
    KEYBOARD_MODE,  /**< The user has opened the virtual keyboard. */
    GESTURE_MODE,    /**< Gesture sequences should result in specific keyboard commands. */
    GESTURE_HOLD_ONE, /**< In Gesture mode, specific gesture has been held */
    GESTURE_HOLD_TWO,
    GESTURE_HOLD_THREE,
    GESTURE_HOLD_FOUR,
    GESTURE_HOLD_FIVE    
};

static std::string modeToString(midasMode mm)
{
    switch (mm)
    {
    case LOCK_MODE:   
#ifdef BUILD_FOR_KARDIUM
        return "LOCKED";
#else
        return "Locked";
#endif
    case MOUSE_MODE:  
#ifdef BUILD_FOR_KARDIUM
        return "UNLOCKED";
#else
        return "Mouse Mouse";
#endif
    case MOUSE_MODE2:
#ifdef BUILD_FOR_KARDIUM
        return "UNLOCKED";
#else
        return "Mouse Mode2";
#endif
    case KEYBOARD_MODE:  
        return "Keyboard Mode";
    case GESTURE_MODE:  
#ifdef BUILD_FOR_KARDIUM
        return "UNLOCKED";
#else
        return "Gesture Mode";
#endif
    case GESTURE_HOLD_ONE:
        return "Hold1 Mode";
    case GESTURE_HOLD_TWO:
        return "Hold2 Mode";
    case GESTURE_HOLD_THREE:
        return "Hold3 Mode";
    case GESTURE_HOLD_FOUR:
        return "Hold4 Mode";
    case GESTURE_HOLD_FIVE:
        return "Hold5 Mode";
    default:
        return "default";
    }
}

/**
 * A status used in the Keyboard Controller, with different
 * error types that can be returned.
 */
enum kybdStatus {
    SUCCESS,                    /**< No error. */
    ARRAY_TOO_SMALL,            /**< A provided buffer is too small for the requested data. */
    N_PRESSES_MATCH_RELEASES    /**< The number of key presses does not match the number of key releases.*/
};

typedef union action{
	kybdCmds kybd;
	kybdGUICmds kybdGUI;
	mouseCmds mouse;
	midasMode mode;
	profileCmds profile;
} action;

/**
 * A simple point consisting of two coordinates, x and y.
 */
struct point {
    int x, y;
    point(int xVal = 0, int yVal = 0) : x(xVal), y(yVal) { }
};

struct vector2D {
	double x, y;
    vector2D(double xVal = 0, double yVal = 0) : x(xVal), y(yVal) { }
};

struct vector3D {
	double x, y, z;
	vector3D(double xVal = 0.0, double yVal = 0.0, double zVal = 0.0) : x(xVal), y(yVal), z(zVal) { }
};

struct keyboardAngle {
    int angle;
    bool ringThreshReached = true;
    
    // below are temporary values for debugging purposes!
    int x, y;
};

enum HoldModeActionType {
    ABS_DELTA_FINITE, // Based on the net delta beyond threshold, ensure the number of +/- actions is at a certain value (repeatable change)
    ABS_DELTA_VELOCITY, // Based on net delta beyond threshold, continue spewing actions
    INTERVAL_DELTA // if in a given interval an angle is +/- beyond a threshold, >=1 action will ensue 
};

#endif /* _MIDAS_COMMON_H */