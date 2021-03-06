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
#include <math.h>
#include <Windows.h>

MyoState::MyoState()
{
	spatialHistLen = 1;
	poseHistLen = 1;
    currentArm = myo::Arm::armRight;
    currentWarmupState = myo::WarmupState::warmupStateWarm;
    currentXDirection = myo::XDirection::xDirectionTowardElbow;
    currentXRotation = 0;
    currentXRotationMatrix = create2DArray(ROTATION_MATRIX_SIZE, ROTATION_MATRIX_SIZE);
    Sleep(100);
    desiredXRotationValue = DEFAULT_DESIRED_X_ROTATION_FW_1_4_1670;

}

MyoState::~MyoState()
{
    if (currentXRotationMatrix != NULL)
    {
        for (int h = 0; h < ROTATION_MATRIX_SIZE; h++)
        {
            delete[] currentXRotationMatrix[h];
        }
        delete[] currentXRotationMatrix;
        currentXRotationMatrix = NULL;
    }
}

void MyoState::checkFWAndUpdate()
{
    if (MyoDevice::getHighestFirmwareVersionConnected().firmwareVersionMajor == 1 &&
        MyoDevice::getHighestFirmwareVersionConnected().firmwareVersionMinor == 4 &&
        MyoDevice::getHighestFirmwareVersionConnected().firmwareVersionPatch == 1670)
    {
        desiredXRotationValue = DEFAULT_DESIRED_X_ROTATION_FW_1_4_1670;
    }
    else
    {
        desiredXRotationValue = DEFAULT_DESIRED_X_ROTATION_FW_1_5_1931;
    }
}

// code taken from http://stackoverflow.com/questions/8617683/return-a-2d-array-from-a-function
float** MyoState::create2DArray(unsigned height, unsigned width)
{
    float** array2D = 0;
    array2D = new float*[height];

    for (int h = 0; h < height; h++)
    {
        array2D[h] = new float[width];

        for (int w = 0; w < width; w++)
        {
            array2D[h][w] = 0.0f;
        }
    }

    return array2D;
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

myo::Quaternion<float> MyoState::peakFrontRotation()
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

myo::Vector3<float> MyoState::peakFrontAccel()
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

myo::Vector3<float> MyoState::peakFrontGryo()
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

myo::Pose MyoState::peakFrontPose()
{
	myoStateMutex.lock();
	myo::Pose front = poseHistory.front();

	myoStateMutex.unlock();

	return front;
}

myo::Pose MyoState::mostRecentPose()
{
    myoStateMutex.lock();
    myo::Pose front;
    if (poseHistory.size() > 0)
    {
        front = poseHistory.back();
    }
    else
    {
        front = myo::Pose::rest;
    }

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

void MyoState::setXRotation(float xRotation)
{
    myoStateMutex.lock();
    this->currentXRotation = xRotation;

    // update rotation matrix
    float rotationAngle = xRotation - desiredXRotationValue;
    // first row
    currentXRotationMatrix[0][0] = 1.0f;
    currentXRotationMatrix[0][1] = 0.0f;
    currentXRotationMatrix[0][2] = 0.0f;
    // second row
    currentXRotationMatrix[1][0] = 0.0f;
    currentXRotationMatrix[1][1] = cos(rotationAngle);
    currentXRotationMatrix[1][2] = -sin(rotationAngle);
    // third row
    currentXRotationMatrix[2][0] = 0.0f;
    currentXRotationMatrix[2][1] = sin(rotationAngle);
    currentXRotationMatrix[2][2] = cos(rotationAngle);

    myoStateMutex.unlock();
}

float MyoState::getXRotation()
{
    myoStateMutex.lock();
    float retVal = this->currentXRotation;
    myoStateMutex.unlock();
    return retVal;
}

float** MyoState::getXRotationMatrix()
{
    return currentXRotationMatrix;
}

void MyoState::setDesiredXRotation(float rotation)
{
    myoStateMutex.lock();
    this->desiredXRotationValue = rotation;

    // update rotation matrix
    float rotationAngle = this->currentXRotation - desiredXRotationValue;
    // first row
    currentXRotationMatrix[0][0] = 1.0f;
    currentXRotationMatrix[0][1] = 0.0f;
    currentXRotationMatrix[0][2] = 0.0f;
    // second row
    currentXRotationMatrix[1][0] = 0.0f;
    currentXRotationMatrix[1][1] = cos(rotationAngle);
    currentXRotationMatrix[1][2] = -sin(rotationAngle);
    // third row
    currentXRotationMatrix[2][0] = 0.0f;
    currentXRotationMatrix[2][1] = sin(rotationAngle);
    currentXRotationMatrix[2][2] = cos(rotationAngle);

    myoStateMutex.unlock();
}

float MyoState::getDesiredXRotation()
{
    myoStateMutex.lock();
    float retVal = this->desiredXRotationValue;
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