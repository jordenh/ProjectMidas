#include "MyoDevice.h"
#include "GestureFilter.h"
#include "MyoTranslationFilter.h"
#include "AveragingFilter.h"
#include "SharedCommandData.h"
#include "EMGImpulseFilter.h"
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
    posePipeline.registerFilter(&gestureFilter);
    posePipeline.registerFilter(WearableDevice::sharedData);

    AveragingFilter averagingFilter(5);
    MyoTranslationFilter translationFilter(state, myoState, mainGui);
    orientationPipeline.registerFilter(&averagingFilter);
    orientationPipeline.registerFilter(&translationFilter);
    orientationPipeline.registerFilter(WearableDevice::sharedData);

	// init profileSignaller to the first profile name.
//	profileSignaller.setProfileName(profileManager->getProfiles()->at(0).profileName);
	profileSignaller.setControlStateHandle(state);
	state->setProfile(profileManager->getProfiles()->at(0).profileName);
	mainGui->connectSignallerToProfileWidgets(&profileSignaller); 
		
    AveragingFilter rssiAveragingFilter(5);
    rssiPipeline.registerFilter(&rssiAveragingFilter);
    rssiPipeline.registerFilter(WearableDevice::sharedData);

    connectPipeline.registerFilter(WearableDevice::sharedData);

    EMGImpulseFilter emgImpulseFilter(myoState);
    emgImpulsePipeline.registerFilter(&emgImpulseFilter);
    emgImpulsePipeline.registerFilter(WearableDevice::sharedData);

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
        parent.posePipeline.startPipeline(input);
    }

    input[GESTURE_INPUT] = pose.type();
    parent.posePipeline.startPipeline(input);

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

    input[INPUT_ARM] = parent.arm;
    input[INPUT_X_DIRECTION] = parent.xDirection;

    // The following is junk data. The averaging filter should be modified so
    // that it doesn't deal with the data so specifically.
    input[ACCEL_DATA_X] = 0.0f;
    input[ACCEL_DATA_Y] = 0.0f;
    input[ACCEL_DATA_Z] = 0.0f;
    input[GYRO_DATA_X] = 0.0f;
    input[GYRO_DATA_Y] = 0.0f;
    input[GYRO_DATA_Z] = 0.0f;
    input[RSSI] = (int8_t)0;
	    
    parent.orientationPipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel)
{
    filterDataMap input;
    input[ACCEL_DATA_X] = accel.x();
    input[ACCEL_DATA_Y] = accel.y();
    input[ACCEL_DATA_Z] = accel.z();

    // The following is junk data. The averaging filter should be modified so
    // that it doesn't deal with the data so specifically.
    input[QUAT_DATA_X] = 0.0f;
    input[QUAT_DATA_Y] = 0.0f;
    input[QUAT_DATA_Z] = 0.0f;
    input[QUAT_DATA_W] = 0.0f;
    input[GYRO_DATA_X] = 0.0f;
    input[GYRO_DATA_Y] = 0.0f;
    input[GYRO_DATA_Z] = 0.0f;
    input[RSSI] = (int8_t)0;
    input[INPUT_ARM] = parent.arm;
    input[INPUT_X_DIRECTION] = parent.xDirection;

    //parent.orientationPipeline.startPipeline(input); //TODO - solve race condition and enable this
}

void MyoDevice::MyoCallbacks::onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro) 
{
    filterDataMap input;
    input[GYRO_DATA_X] = gyro.x();
    input[GYRO_DATA_Y] = gyro.y();
    input[GYRO_DATA_Z] = gyro.z();

    // The following is junk data. The averaging filter should be modified so
    // that it doesn't deal with the data so specifically.
    input[QUAT_DATA_X] = 0.0f;
    input[QUAT_DATA_Y] = 0.0f;
    input[QUAT_DATA_Z] = 0.0f;
    input[QUAT_DATA_W] = 0.0f;
    input[ACCEL_DATA_X] = 0.0f;
    input[ACCEL_DATA_Y] = 0.0f;
    input[ACCEL_DATA_Z] = 0.0f;
    input[RSSI] = (int8_t)0;
    input[INPUT_ARM] = parent.arm;
    input[INPUT_X_DIRECTION] = parent.xDirection;

    //parent.orientationPipeline.startPipeline(input); //TODO - solve race condition and enable this
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

    parent.connectPipeline.startPipeline(input);

    parent.connectedMyos.push_back(myo);
    //parent.currentMyo = myo;
}
void MyoDevice::MyoCallbacks::onDisconnect(Myo* myo, uint64_t timestamp) { 
    std::cout << "on disconnect." << std::endl; 
    filterDataMap input;
    input[ISCONNECTED_INPUT] = false;

    parent.connectPipeline.startPipeline(input);

    for (std::vector<Myo*>::iterator it = parent.connectedMyos.begin(); it != parent.connectedMyos.end(); it++)
    {
        if (myo == *it)
        {
            parent.connectedMyos.erase(it);
            break;
        }
    }
    //parent.currentMyo = NULL;
}

void MyoDevice::MyoCallbacks::onArmSync(Myo *myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState)
{
    parent.arm = arm;
    parent.xDirection = xDirection;
    std::cout << "on arm sync." << std::endl;
}
void MyoDevice::MyoCallbacks::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) { 
    parent.arm = arm;
    parent.xDirection = xDirection;
    std::cout << "on arm sync." << std::endl; 
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
    // This data is streaming at 200Hz - print data for post mortem analysis
	std::cout << "onEmgData." << std::endl;

    for (int emgIdx = 0; emgIdx < 8; emgIdx++)
    {
        lastEMGData[emgIdx] = emg[emgIdx];
    }
//    printToDataFile(); // uncomment to printout data for post mortem analysis

    // Actual processing
    std::array<int8_t, 8> emgSamples;
    for (int i = 0; i < 8; i++) {
        emgSamples[i] = emg[i];
    }

    filterDataMap input;
    input[EMG_VECTOR] = emgSamples;

    parent.emgImpulsePipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onWarmupCompleted(myo::Myo* myo, uint64_t timestamp, WarmupResult warmupResult)
{
	std::cout << "onWarmupCompleted." << std::endl;
}

void MyoDevice::MyoCallbacks::onRssi(Myo* myo, uint64_t timestamp, int8_t rssi) {
	std::cout << "on rssi." << std::endl;
	filterDataMap input;
	input[RSSI] = rssi;

	// The following is junk data. The averaging filter should be modified so
	// that it doesn't deal with the data so specifically.
	input[GYRO_DATA_X] = 0.0f;
	input[GYRO_DATA_Y] = 0.0f;
	input[GYRO_DATA_Z] = 0.0f;
	input[QUAT_DATA_X] = 0.0f;
	input[QUAT_DATA_Y] = 0.0f;
	input[QUAT_DATA_Z] = 0.0f;
	input[QUAT_DATA_W] = 0.0f;
	input[ACCEL_DATA_X] = 0.0f;
	input[ACCEL_DATA_Y] = 0.0f;
	input[ACCEL_DATA_Z] = 0.0f;
	input[INPUT_ARM] = armUnknown;
	input[INPUT_X_DIRECTION] = xDirectionUnknown;
	parent.rssiPipeline.startPipeline(input);
}

void MyoDevice::updateProfiles(void)
{
    std::list<Filter*>* filters = posePipeline.getFilters();
	
    int error = (int)filterError::NO_FILTER_ERROR;
    for (std::list<Filter*>::iterator it = filters->begin(); it != filters->end(); ++it)
    {
		error |= (int)(*it)->updateBasedOnProfile(*profileManager, state->getProfile());//profileSignaller.getProfileName());
    }
	
    filters = orientationPipeline.getFilters();
    for (std::list<Filter*>::iterator it = filters->begin(); it != filters->end(); ++it)
    {
		error |= (int)(*it)->updateBasedOnProfile(*profileManager, state->getProfile());// profileSignaller.getProfileName());
    }
	
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