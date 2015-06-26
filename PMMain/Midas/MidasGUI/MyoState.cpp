#include "MyoState.h"
#include "MyoDevice.h"

MyoState::MyoState()
{
	spatialHistLen = 1;
	poseHistLen = 1;
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
{ myoHandle = myo; }

const MyoDevice* MyoState::peakMyo()
{ return myoHandle; }

bool MyoState::lastPoseNonRest()
{
    if (poseHistory.size() >= 1 &&
        Pose::rest != poseHistory.back())
    {
        return true;
    }
    else
    {
        return false;
    }
}