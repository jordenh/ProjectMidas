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

// SCD Keys
#define COMMAND_INPUT "command"
#define VELOCITY_INPUT "velocity"
#define ISCONNECTED_INPUT "isConnected"
#define SYNCHED_INPUT "isSynched"
#define DELTA_INPUT "deltaInput"
#define EMG_VECTOR "emgVector"
#define IMPULSE_STATUS "impStatus"
#define ANGLE_INPUT "angle"
#define RSSI_INPUT "rssi"
#define BATTERY_LEVEL_INPUT "batteryLevel"

// Myo Translation Keys
#define INPUT_QUATERNION_X "quatDataX"
#define INPUT_QUATERNION_Y "quatDataY"
#define INPUT_QUATERNION_Z "quatDataZ"
#define INPUT_QUATERNION_W "quatDataW"
#define INPUT_ARM "arm"
#define INPUT_X_DIRECTION "xDirection"

// Averaging Keys
#define QUAT_DATA_X "quatDataX"
#define QUAT_DATA_Y "quatDataY"
#define QUAT_DATA_Z "quatDataZ"
#define QUAT_DATA_W "quatDataW"
#define ACCEL_DATA_X "accelDataX"
#define ACCEL_DATA_Y "accelDataY"
#define ACCEL_DATA_Z "accelDataZ"
#define GYRO_DATA_X "gyroDataX"
#define GYRO_DATA_Y "gyroDataY"
#define GYRO_DATA_Z "gyroDataZ"
#define RSSI        "rssi"

// Gesture Recognition Keys
#define GESTURE_INPUT "gesture"
#define GESTURE_FILTER_STATE_CHANGE "gfStateChange" // work around to send a signal to the gesture filter to change a state directly, since it is setup to do so.