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

#include "MyoState.h"
#include "MyoDevice.h"

MyoState::MyoState()
{
	spatialHistLen = 1;
	poseHistLen = 1;
    currentArm = myo::Arm::armRight;
    currentWarmupState = myo::WarmupState::warmupStateWarm;
    currentXDirection = myo::XDirection::xDirectionTowardElbow;
}

void MyoState::setSpatialHistLen(int spatialHistLen)
{
	myoStateMutex.lock();
	this->spatialHistLen = spatialHistLen;
	myoStateMutex.unlock();
}

void MyoState::setPoseHistLen(int poseHistLen)
{
	myoStateMutex.lock();
	this->poseHistLen = poseHistLen;
	myoStateMutex.unlock();
}

void MyoState::pushRotation(myo::Quaternion<float> rotation)
{
	myoStateMutex.lock();
	rotationHistory.push_back(rotation);

	if (rotationHistory.size() > this->spatialHistLen)
	{
		rotationHistory.pop_front();
	}
	myoStateMutex.unlock();
}

myo::Quaternion<float> MyoState::popRotation()
{
	myoStateMutex.lock();
	myo::Quaternion<float> front = rotationHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::deque<myo::Quaternion<float>> MyoState::getRotationHistory()
{
	myoStateMutex.lock();
	std::deque<myo::Quaternion<float>> copy = std::deque<myo::Quaternion<float>>(rotationHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::pushAccel(myo::Vector3<float> accel)
{
	myoStateMutex.lock();
	accelHistory.push_back(accel);

	if (accelHistory.size() > this->spatialHistLen)
	{
		accelHistory.pop_front();
	}
	myoStateMutex.unlock();
}

myo::Vector3<float> MyoState::popAccel()
{
	myoStateMutex.lock();
	myo::Vector3<float> front = accelHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::deque<myo::Vector3<float>> MyoState::getAccelHistory()
{
	myoStateMutex.lock();
	std::deque<myo::Vector3<float>> copy = std::deque<myo::Vector3<float>>(accelHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::pushGyro(myo::Vector3<float> gyro)
{
	myoStateMutex.lock();
	gyroHistory.push_back(gyro);

	if (gyroHistory.size() > this->spatialHistLen)
	{
		gyroHistory.pop_front();
	}
	myoStateMutex.unlock();
}

myo::Vector3<float> MyoState::popGryo()
{
	myoStateMutex.lock();
	myo::Vector3<float> front = gyroHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::deque<myo::Vector3<float>> MyoState::getGyroHistory()
{
	myoStateMutex.lock();
	std::deque<myo::Vector3<float>> copy = std::deque<myo::Vector3<float>>(gyroHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::pushPose(myo::Pose pose)
{
	myoStateMutex.lock();
	poseHistory.push_back(pose);

	if (poseHistory.size() > this->poseHistLen)
	{
		poseHistory.pop_front();
	}
	myoStateMutex.unlock();
}

myo::Pose MyoState::popPose()
{
	myoStateMutex.lock();
	myo::Pose front = poseHistory.front();

	myoStateMutex.unlock();

	return front;
}

std::deque<myo::Pose> MyoState::getPoseHistory()
{
	myoStateMutex.lock();
	std::deque<myo::Pose> copy = std::deque<myo::Pose>(poseHistory);

	myoStateMutex.unlock();

	return copy;
}

void MyoState::setMyo(MyoDevice *myo)
{ 
    myoStateMutex.lock();
    myoHandle = myo; 
    myoStateMutex.unlock();
}

const MyoDevice* MyoState::peakMyo()
{ 
    return myoHandle; 
}

void MyoState::setArm(myo::Arm arm)
{
    myoStateMutex.lock();
    this->currentArm = arm;
    myoStateMutex.unlock();
}

myo::Arm MyoState::getArm()
{
    myoStateMutex.lock();
    myo::Arm retVal = this->currentArm;
    myoStateMutex.unlock();
    return retVal;
}

void MyoState::setWarmupState(myo::WarmupState warmupState)
{
    myoStateMutex.lock();
    this->currentWarmupState = warmupState;
    myoStateMutex.unlock();
}

myo::WarmupState MyoState::getWarmupState()
{
    myoStateMutex.lock();
    myo::WarmupState retVal = this->currentWarmupState;
    myoStateMutex.unlock();
    return retVal;
}

void MyoState::setXDirection(myo::XDirection xDirection)
{
    myoStateMutex.lock();
    this->currentXDirection = xDirection;
    myoStateMutex.unlock();
}

myo::XDirection MyoState::getXDirection()
{
    myoStateMutex.lock();
    myo::XDirection retVal = this->currentXDirection;
    myoStateMutex.unlock();
    return retVal;
}

bool MyoState::lastPoseNonRest()
{
    myoStateMutex.lock();
    if (poseHistory.size() >= 1 &&
        Pose::rest != poseHistory.back())
    {
        myoStateMutex.unlock();
        return true;
    }
    else
    {
        myoStateMutex.unlock();
        return false;
    }
}