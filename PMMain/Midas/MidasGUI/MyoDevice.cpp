#include "MyoDevice.h"
#include "GestureFilter.h"
#include "FilterKeys.h"
#include "MyoTranslationFilter.h"
#include "GenericAveragingFilter.h"
#include "GenericBypassFilter.h"
#include "SharedCommandData.h"
#include "ControlState.h"
#include "MyoState.h"
#include "MainGUI.h"
#include "ProfileManager.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <time.h>

#define MIN_RSSI_DELAY 3000
#define MIN_BATTERY_LEVEL_DELAY 30000

ProfileSignaller MyoDevice::profileSignaller;

MyoDevice::MyoDevice(SharedCommandData* sharedCommandData, ControlState* controlState, MyoState* myoState,
    std::string applicationIdentifier, MainGUI *mainGuiHandle, ProfileManager *profileManagerHandle)
    : WearableDevice(sharedCommandData), appIdentifier(applicationIdentifier), myoFindTimeout(DEFAULT_FIND_MYO_TIMEOUT),
    durationInMilliseconds(DEFAULT_MYO_DURATION_MS), state(controlState), myoState(myoState), arm(DEFAULT_MYO_ARM), 
	xDirection(DEFAULT_MYO_XDIR), mainGui(mainGuiHandle), profileManager(profileManagerHandle)
{
    prevProfileName = "";
}

MyoDevice::~MyoDevice()
{
    delete genAvgFilterRSSI; genAvgFilterRSSI = NULL;

    delete genAvgFilterQX; genAvgFilterQX = NULL;
    delete genAvgFilterQY; genAvgFilterQY = NULL;
    delete genAvgFilterQZ; genAvgFilterQZ = NULL;
    delete genAvgFilterQW; genAvgFilterQW = NULL;
    
    delete genAvgFilterAY; genAvgFilterAY = NULL;
    delete genAvgFilterAZ; genAvgFilterAZ = NULL;
    delete genAvgFilterAW; genAvgFilterAW = NULL;
    
    delete genAvgFilterGY; genAvgFilterGY = NULL;
    delete genAvgFilterGZ; genAvgFilterGZ = NULL;
    delete genAvgFilterGW; genAvgFilterGW = NULL;

    delete genBypassFilterArm; genBypassFilterArm = NULL;
    delete genBypassFilterXDir; genBypassFilterXDir = NULL;

    delete translationFilter; translationFilter = NULL;
}

void MyoDevice::setFindMyoTimeout(unsigned int milliseconds)
{
    myoFindTimeout = milliseconds;
}

void MyoDevice::setMyoEventLoopDuration(unsigned int milliseconds)
{
    durationInMilliseconds = milliseconds;
}

void MyoDevice::runDeviceLoop()
{
    WearableDevice::setDeviceStatus(deviceStatus::RUNNING);

    GestureFilter gestureFilter(state, myoState, 0, mainGui);
    setupPosePipeline(&gestureFilter);

    setupOrientationPipeline();

    setupRSSIPipeline();

    advancedConnectPipeline.registerFilterAtDeepestLevel(WearableDevice::sharedData);

	profileSignaller.setControlStateHandle(state);
	state->setProfile(profileManager->getProfiles()->at(0).profileName);
	mainGui->connectSignallerToProfileWidgets(&profileSignaller); 

    std::chrono::milliseconds rssi_start =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()); /* Used to control when to request rssi */
    std::chrono::milliseconds current_time;

	std::chrono::milliseconds battery_start =
		std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()); /* Used to control when to request battery levels */

    try
    {
        Hub hub(appIdentifier);
        hub.setLockingPolicy(hub.lockingPolicyNone);
        Myo* myo = hub.waitForMyo(myoFindTimeout);

        if (!myo)
        {
            std::cout << "Could not find a Myo." << std::endl;
            WearableDevice::setDeviceStatus(deviceStatus::ERR);
            return;
        }

        // Enable EMG streaming on the found Myo.
        myo->setStreamEmg(myo::Myo::streamEmgEnabled);

        MyoCallbacks myoCallbacks(*this);
        hub.addListener(&myoCallbacks);

        while (true)
        {
            if (WearableDevice::stopDeviceRequested()) 
            {
                break;
            }

            filterDataMap extraData = gestureFilter.getExtraDataForSCD();
            if (extraData.size() > 0)
            {
                WearableDevice::sharedData->setInput(extraData);
                WearableDevice::sharedData->process();
            }

			if (state->getProfile() != prevProfileName)
            {
				prevProfileName = state->getProfile();
                updateProfiles();
            }

			current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch());
			if ((current_time - rssi_start).count() > MIN_RSSI_DELAY)
            {
                myo->requestRssi();
                rssi_start = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch());
            }

			if ((current_time - battery_start).count() > MIN_BATTERY_LEVEL_DELAY)
			{
				myo->requestBatteryLevel();
				battery_start = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now().time_since_epoch());
			}

            hub.run(durationInMilliseconds);
            
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        WearableDevice::setDeviceStatus(deviceStatus::ERR);
        return;
    }

    WearableDevice::setDeviceStatus(deviceStatus::DONE);
}

void MyoDevice::setupPosePipeline(GestureFilter *gf)
{
    advancedPosePipeline.registerFilterAtDeepestLevel(gf);

    advancedPosePipeline.registerFilterAtNewLevel(WearableDevice::sharedData);
}

void MyoDevice::setupOrientationPipeline()
{
    genAvgFilterQX = new GenericAveragingFilter(5, QUAT_DATA_X);
    genAvgFilterQY = new GenericAveragingFilter(5, QUAT_DATA_Y);
    genAvgFilterQZ = new GenericAveragingFilter(5, QUAT_DATA_Z);
    genAvgFilterQW = new GenericAveragingFilter(5, QUAT_DATA_W);

    genAvgFilterAY = new GenericAveragingFilter(5, ACCEL_DATA_X);
    genAvgFilterAZ = new GenericAveragingFilter(5, ACCEL_DATA_Y);
    genAvgFilterAW = new GenericAveragingFilter(5, ACCEL_DATA_Z);

    genAvgFilterGY = new GenericAveragingFilter(5, GYRO_DATA_X);
    genAvgFilterGZ = new GenericAveragingFilter(5, GYRO_DATA_Y);
    genAvgFilterGW = new GenericAveragingFilter(5, GYRO_DATA_Z);

    genBypassFilterArm = new GenericBypassFilter(INPUT_ARM);
    genBypassFilterXDir = new GenericBypassFilter(INPUT_X_DIRECTION);

    translationFilter = new MyoTranslationFilter(state, myoState, mainGui);

    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterQX);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterQY);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterQZ);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterQW);

    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterAY);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterAZ);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterAW);

    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterGY);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterGZ);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genAvgFilterGW);

    advancedOrientationPipeline.registerFilterAtDeepestLevel(genBypassFilterArm);
    advancedOrientationPipeline.registerFilterAtDeepestLevel(genBypassFilterXDir);

    advancedOrientationPipeline.registerFilterAtNewLevel(translationFilter);

    advancedOrientationPipeline.registerFilterAtNewLevel(WearableDevice::sharedData);
}

void MyoDevice::setupRSSIPipeline()
{
    genAvgFilterRSSI = new GenericAveragingFilter(5, RSSI);

    advancedRssiPipeline.registerFilterAtDeepestLevel(genAvgFilterRSSI);

    advancedRssiPipeline.registerFilterAtNewLevel(WearableDevice::sharedData);
}

int MyoDevice::getDeviceError()
{
    // TODO: Add error codes.
    return 0;
}

void MyoDevice::vibrateMyos(myo::Myo::VibrationType vibType, int numReps) const
{
    for (int i = 0; i < connectedMyos.size(); i++)
    {
        Myo* myo = connectedMyos[i];
        if (myo != NULL) {
            for (int j = 0; j < numReps; j++)
            {
                myo->vibrate(vibType);
            }
        }
    }
}

// Device Listener
MyoDevice::MyoCallbacks::MyoCallbacks(MyoDevice& parentDevice) 
    : parent(parentDevice)
{
    myoDataFile.open("myoDataFile.csv");
    lastPose = Pose::rest;
}

MyoDevice::MyoCallbacks::~MyoCallbacks() { myoDataFile.close(); }

// Overridden functions from DeviceListener
void MyoDevice::MyoCallbacks::onPose(Myo* myo, uint64_t timestamp, Pose pose) 
{
    filterDataMap input;

    if (pose != Pose::rest && parent.myoState->lastPoseNonRest())
    {
        /* HACK - Myo API used to enforce that gestures ALWAYS had a 'rest' gesture inbetween other poses.
        * Going to manually insert a rest inbetween sequences without a rest.
        */
        input[GESTURE_INPUT] = Pose::rest;
        parent.advancedPosePipeline.startPipeline(input);
    }

    input[GESTURE_INPUT] = pose.type();
    parent.advancedPosePipeline.startPipeline(input);

    lastPose = pose.type();
    printToDataFile();
}

void MyoDevice::MyoCallbacks::onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation) 
{ 
    filterDataMap input;

    input[QUAT_DATA_X] = rotation.x();
    input[QUAT_DATA_Y] = rotation.y();
    input[QUAT_DATA_Z] = rotation.z();
    input[QUAT_DATA_W] = rotation.w();
	    
    parent.advancedOrientationPipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel)
{
    filterDataMap input;
    input[ACCEL_DATA_X] = accel.x();
    input[ACCEL_DATA_Y] = accel.y();
    input[ACCEL_DATA_Z] = accel.z();

    // For now, advancedOrientationPipeline doesnt use this data, so not going
    // to bother starting pipeline. Uncomment (and it will work fine) if data
    // is handled in the future.
    //parent.advancedOrientationPipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro) 
{
    filterDataMap input;
    input[GYRO_DATA_X] = gyro.x();
    input[GYRO_DATA_Y] = gyro.y();
    input[GYRO_DATA_Z] = gyro.z();

    // For now, advancedOrientationPipeline doesnt use this data, so not going
    // to bother starting pipeline. Uncomment (and it will work fine) if data
    // is handled in the future.
    //parent.advancedOrientationPipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) { 
    std::cout << "on pair." << std::endl; 
    // don't set myo from this callback - onPair executes even when no Myo's are connected
    // to Myo-connect for some reason
}
void MyoDevice::MyoCallbacks::onUnpair(Myo* myo, uint64_t timestamp) { 
    std::cout << "on unpair." << std::endl; 
}
void MyoDevice::MyoCallbacks::onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion) { 
    std::cout << "on connect." << std::endl; 
    filterDataMap input;
    input[ISCONNECTED_INPUT] = true;

    parent.advancedConnectPipeline.startPipeline(input);

    parent.connectedMyos.push_back(myo);
}
void MyoDevice::MyoCallbacks::onDisconnect(Myo* myo, uint64_t timestamp) { 
    std::cout << "on disconnect." << std::endl; 
    filterDataMap input;
    input[ISCONNECTED_INPUT] = false;

    parent.advancedConnectPipeline.startPipeline(input);

    for (std::vector<Myo*>::iterator it = parent.connectedMyos.begin(); it != parent.connectedMyos.end(); it++)
    {
        if (myo == *it)
        {
            parent.connectedMyos.erase(it);
            break;
        }
    }
}

void MyoDevice::MyoCallbacks::onArmSync(Myo *myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState)
{
    parent.arm = arm;
    parent.xDirection = xDirection;
    std::cout << "on arm sync." << std::endl;

    filterDataMap input;

    input[INPUT_ARM] = parent.arm;
    input[INPUT_X_DIRECTION] = parent.xDirection;

    parent.advancedOrientationPipeline.startPipeline(input);
}
void MyoDevice::MyoCallbacks::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) { 
    parent.arm = arm;
    parent.xDirection = xDirection;
    std::cout << "on arm sync." << std::endl; 

    filterDataMap input;

    input[INPUT_ARM] = parent.arm;
    input[INPUT_X_DIRECTION] = parent.xDirection;

    parent.advancedOrientationPipeline.startPipeline(input);
}
void MyoDevice::MyoCallbacks::onArmUnsync(Myo* myo, uint64_t timestamp) { 
    parent.arm = Arm::armUnknown;
    parent.xDirection = XDirection::xDirectionUnknown;
    std::cout << "on arm unsync." << std::endl; 
}

void MyoDevice::MyoCallbacks::onUnlock(Myo* myo, uint64_t timestamp)
{
	std::cout << "on unlock." << std::endl;
}

void MyoDevice::MyoCallbacks::onLock(Myo* myo, uint64_t timestamp)
{
	std::cout << "on lock." << std::endl;
}

void MyoDevice::MyoCallbacks::onBatteryLevelReceived(myo::Myo* myo, uint64_t timestamp, uint8_t level)
{
	std::cout << "onBatteryLevelReceived." << std::endl;
}

void MyoDevice::MyoCallbacks::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
    // This data is streaming at 200Hz
	std::cout << "onEmgData." << std::endl;

    for (int emgIdx = 0; emgIdx < 8; emgIdx++)
    {
        lastEMGData[emgIdx] = emg[emgIdx];
    }
    printToDataFile();
}

void MyoDevice::MyoCallbacks::onWarmupCompleted(myo::Myo* myo, uint64_t timestamp, WarmupResult warmupResult)
{
	std::cout << "onWarmupCompleted." << std::endl;
}

void MyoDevice::MyoCallbacks::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) {
	std::cout << "on rssi." << std::endl;
	filterDataMap input;
	input[RSSI] = static_cast<float>(rssi);

    parent.advancedRssiPipeline.startPipeline(input);
}

void MyoDevice::updateProfiles(void)
{
    int error = (int)filterError::NO_FILTER_ERROR;

    error |= (int)advancedPosePipeline.updateFiltersBasedOnProfile(*profileManager, state->getProfile());
	
    error |= (int)advancedOrientationPipeline.updateFiltersBasedOnProfile(*profileManager, state->getProfile());
    	
    if (error != (int)filterError::NO_FILTER_ERROR)
    {
        throw new std::exception("updateProfileException");
    }
}

void MyoDevice::MyoCallbacks::printToDataFile()
{
    for (int emgIdx = 0; emgIdx < 8; emgIdx++)
    {
        myoDataFile << static_cast<int>(lastEMGData[emgIdx]) << ",";
    }
    myoDataFile << ((int)lastPose) << std::endl;
}