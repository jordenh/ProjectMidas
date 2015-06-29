#ifndef MYO_STATE_H
#define MYO_STATE_H

#include <deque>
#include <mutex>
#include "myo\cxx\Vector3.hpp"
#include "myo\cxx\Quaternion.hpp"
#include "myo\cxx\Pose.hpp"

class MyoDevice;

class MyoState {
public:
	MyoState();

	void setSpatialHistLen(int spatialHistLen);
	void setPoseHistLen(int poseHistLen);

	void pushRotation(myo::Quaternion<float> rotation);
	myo::Quaternion<float> peakFrontRotation();
	std::deque<myo::Quaternion<float>> getRotationHistory();

	void pushAccel(myo::Vector3<float> accel);
	myo::Vector3<float> peakFrontAccel();
	std::deque<myo::Vector3<float>> getAccelHistory();

	void pushGyro(myo::Vector3<float> gyro);
	myo::Vector3<float> peakFrontGryo();
	std::deque<myo::Vector3<float>> getGyroHistory();

	void pushPose(myo::Pose pose);
	myo::Pose peakFrontPose();
	std::deque<myo::Pose> getPoseHistory();

    void setMyo(MyoDevice *myo);
    const MyoDevice* peakMyo();

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

    MyoDevice* myoHandle;
};

#endif /* MYO_STATE_H */