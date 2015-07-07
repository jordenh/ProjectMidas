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

#ifndef _MYO_TRANSLATION_FILTER_H
#define _MYO_TRANSLATION_FILTER_H

#include "Filter.h"
#include "FilterKeys.h"
#include "GestureHoldModeAction.h"
#include "ProfileManager.h"
#include "SettingsSignaller.h"
#include "myo\myo.hpp"
#include <math.h>

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

class MyoState;
class ControlState;
class MainGUI;
class HoldModeObserver;

#define MAX_PITCH_ANGLE 25.0f /* Maximum delta angle in degrees */
#define MAX_YAW_ANGLE 30.0f /* Maximum delta angle in degrees */

#define KEYBOARD_THRESH_MAG 30

#define NUM_GESTURES 5
#define GESTURE_DOUBLE_TAP 0 // used as indexes into gestHoldModeAction
#define GESTURE_FINGERS_SPREAD 1
#define GESTURE_FIST 2
#define GESTURE_WAVE_IN 3
#define GESTURE_WAVE_OUT 4

/**
 * Consult Filter.h for concepts regarding Filters.
 * Handles translating from Myo orientation data to mouse movement information.
 */
class MyoTranslationFilter : public Filter
{
public:
    /**
     * The constructor for MyoTranslationFilter.
     *
     * @param controlState A handle to ControlState, to keep track of the application state.
     */
    MyoTranslationFilter(ControlState* controlState, MyoState* myoState, MainGUI *mainGuiHandle);
    ~MyoTranslationFilter();

    /**
     * Convert the supplied orientation data into mouse movement information, in the form of a
     * percent of the total mouse velocity along both the x and y axes. 
     */
    void process();

    /**
    * Calculates the delta (in radians) between a base angle and a
    * current angle, with respect to a ring. The pupose is to ensure that wrapping
    * around the crossover section of the ring has no effect on the output
    * which should safely range from -pi to +pi.
    *
    * @param current The current angle (in radians) that is being compared (from -Pi to pi rad)
    * @param base The base angle (in radians) that is being compared against
    * @return a value from -pi to +pi representing the delta between two input angles
    */
    static float calcRingDelta(float current, float base);

    static float radToDeg(float rad)
    {
        return rad * (180.0 / M_PI);
    }

    static float degToRad(float deg)
    {
        return (deg / 180.0) * M_PI;
    }

    filterError updateBasedOnProfile(ProfileManager& pm, std::string name);

private:
    void handleQuatData(filterDataMap input, filterDataMap output);

    void handleArmData(filterDataMap input, filterDataMap output);

    void handleXDirectionData(filterDataMap input, filterDataMap output);

    /**
     * Calculate the 'pitch' angle from the supplied quaternion, consisting of x, y, z and w,
     * and infer sign based on arm and xDirection.
     *
     * @param x The x coordinate of the quaternion.
     * @param y The y coordinate of the quaternion.
     * @param z The z coordinate of the quaternion.
     * @param w The w coordinate of the quaternion.
     * @param arm The arm of the user, which is accounted for by sign of calculated Pitch.
     * @param xDirection The xDirection of the user, which is accounted for by sign of calculated Pitch.
     * @return Returns the 'pitch' angle, in radians.
     */
    float getPitchFromQuaternion(float x, float y, float z, float w, Arm arm, XDirection xDirection);

    /**
    * Calculate the 'yaw' angle from the supplied quaternion, consisting of x, y, z and w.
    * Unlike pitch, yaw is indifferent to arm/xDirection of Myo.
    *
    * @param x The x coordinate of the quaternion.
    * @param y The y coordinate of the quaternion.
    * @param z The z coordinate of the quaternion.
    * @param w The w coordinate of the quaternion.
    * @return Returns the 'yaw' angle, in radians.
    */
    float getYawFromQuaternion(float x, float y, float z, float w);

    /**
    * Calculate the 'roll' angle from the supplied quaternion, consisting of x, y, z and w.
    * Unlike pitch, roll is indifferent to arm/xDirection of Myo.
    *
    * @param x The x coordinate of the quaternion.
    * @param y The y coordinate of the quaternion.
    * @param z The z coordinate of the quaternion.
    * @param w The w coordinate of the quaternion.
    * @return Returns the 'roll' angle, in radians.
    */
    float getRollFromQuaternion(float x, float y, float z, float w);

    /**
    * Calculates the percent of the mouse cursor's total velocity along the x
    * and y axis, in the form of a point.
    *
    * @param pitch The pitch angle, in radians.
    * @param yaw The yaw angle, in radians.
    * @return A point with the x coordinate being the percent of the velocity along the x axis, 
    * and y being the percent of the total velocity along the y axis.
    */
    point getMouseUnitVelocity(float pitch, float yaw);

	vector2D getMouseDelta(float pitch, float yaw);

    void performHoldModeFunc(unsigned int holdNum, filterDataMap& outputToSharedCommandData);
    void performMouseModeFunc(filterDataMap& outputToSharedCommandData);
    void performeKybdModeFunc(filterDataMap& outputToSharedCommandData);

    bool initGestHoldModeActionArr(void);
    void unregisterHoldModeActions(void);

    ControlState* controlStateHandle; // not owned
    MyoState* myoStateHandle; // not owned
    midasMode previousMode;
    float pitch, basePitch, prevPitch, deltaPitchDeg, 
        yaw, baseYaw, prevYaw, deltaYawDeg,
        roll, baseRoll, prevRoll, deltaRollDeg;

    HoldModeObserver *hmo;
    GestureHoldModeAction gestHoldModeAction[5];

    MainGUI *mainGui; // not owned
    static SettingsSignaller settingsSignaller;
};

#endif