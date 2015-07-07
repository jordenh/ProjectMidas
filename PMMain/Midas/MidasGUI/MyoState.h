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

#ifndef MYO_STATE_H
#define MYO_STATE_H

#include <deque>
#include <mutex>
#include "myo\myo.hpp"

class MyoDevice;

class MyoState {
public:
	MyoState();

	void setSpatialHistLen(int spatialHistLen);
	void setPoseHistLen(int poseHistLen);

	void pushRotation(myo::Quaternion<float> rotation);
	myo::Quaternion<float> popRotation();
	std::deque<myo::Quaternion<float>> getRotationHistory();

	void pushAccel(myo::Vector3<float> accel);
	myo::Vector3<float> popAccel();
	std::deque<myo::Vector3<float>> getAccelHistory();

	void pushGyro(myo::Vector3<float> gyro);
	myo::Vector3<float> popGryo();
	std::deque<myo::Vector3<float>> getGyroHistory();

	void pushPose(myo::Pose pose);
	myo::Pose popPose();
	std::deque<myo::Pose> getPoseHistory();
    myo::Pose mostRecentPose();

    void setMyo(MyoDevice *myo);
    const MyoDevice* peakMyo();

    void setArm(myo::Arm arm);
    myo::Arm getArm();

    void setWarmupState(myo::WarmupState warmupState);
    myo::WarmupState getWarmupState();

    void setXDirection(myo::XDirection xDirection);
    myo::XDirection getXDirection();

    // returns true if the most recent pose is non-rest poses.
    // returns false if size < 1, or last pose is rest.
    bool lastPoseNonRest();

private:
	std::mutex myoStateMutex;

	int spatialHistLen;
	std::deque<myo::Quaternion<float>> rotationHistory;
	std::deque<myo::Vector3<float>> accelHistory;
	std::deque<myo::Vector3<float>> gyroHistory;

	int poseHistLen;
	std::deque<myo::Pose> poseHistory;

    // Misc Myo state values
    myo::Arm currentArm;
    myo::WarmupState currentWarmupState;
    myo::XDirection currentXDirection;

    MyoDevice* myoHandle; // not owned
};

#endif /* MYO_STATE_H */