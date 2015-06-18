#ifndef BASE_MEASUREMENTS_H
#define BASE_MEASUREMENTS_H

#include "MyoState.h"
#include "myo\myo.hpp"

/* Singleton */

class BaseMeasurements {
public:
	static BaseMeasurements& getInstance()
	{
		static BaseMeasurements instance;
		return instance;
	}

	void setBaseAngles(float roll, float pitch, float yaw);

	void setBaseCursor(unsigned int x, unsigned int y);

	// use Windows API to update cursor position
	void updateBaseCursor();

	void setScreenSize(unsigned int x, unsigned int y);

	void setCurrentAngles(float roll, float pitch, float yaw);

	void setCurrentAnglesAsBase();

	void setCurrentPose(myo::Pose pose);

	void setMyoStateHandle(MyoState* myoState) { myoStateHandle = myoState; }

	float getBaseRoll() { return baseRoll; }
	float getBasePitch() { return basePitch; }
	float getBaseYaw() { return baseYaw; }
	float getBaseCursorX() { return baseCursorX; }
	float getBaseCursorY() { return baseCursorY; }
	float getScreenSizeX() { return screenSizeX; }
	float getScreenSizeY() { return screenSizeY; }
	myo::Pose BaseMeasurements::getCurrentPose() { return currentPose; }

	bool areCurrentValuesValid();

private:
	// Force as singleton
	BaseMeasurements() { myoStateHandle = NULL; };
	//~BaseMeasurements();
	BaseMeasurements(BaseMeasurements const&) = delete;
	void operator=(BaseMeasurements const&) = delete;

	std::mutex accessMutex;

	float baseRoll;
	float basePitch;
	float baseYaw;

	unsigned int baseCursorX;
	unsigned int baseCursorY;

	unsigned int screenSizeX;
	unsigned int screenSizeY;

	// helper info
	float currentRoll;
	float currentPitch;
	float currentYaw;

	myo::Pose currentPose;

	MyoState* myoStateHandle;
};

#endif /* BASE_MEASUREMENTS_H */