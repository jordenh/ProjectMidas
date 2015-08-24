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

#include "MyoDevice.h"
#include "GestureFilter.h"
#include "FilterKeys.h"
#include "MyoTranslationFilter.h"
#include "GenericAveragingFilter.h"
#include "GenericBypassFilter.h"
#include "GenericWindowMaxFilter.h"
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

#define MIN_RSSI_DELAY 2000

ProfileSignaller MyoDevice::profileSignaller;

FirmwareVersion MyoDevice::highestFirmwareVersionConnected;
bool isFWVersionHigher(FirmwareVersion a, FirmwareVersion b)
{
    if (a.firmwareVersionMajor > b.firmwareVersionMajor)
    {
        return true;
    }
    else if (a.firmwareVersionMajor == b.firmwareVersionMajor)
    {
        // Majors the same
        if (a.firmwareVersionMinor > b.firmwareVersionMinor)
        {
            return true;
        }
        else if (a.firmwareVersionMinor == b.firmwareVersionMinor)
        {
            // Minors the same
            if (a.firmwareVersionPatch > b.firmwareVersionPatch)
            {
                return true;
            }
            else if (a.firmwareVersionPatch == b.firmwareVersionPatch)
            {
                // Patch the same - so the FW version is the same.
            }
        }
    }

    return false;
}

MyoDevice::MyoDevice(SharedCommandData* sharedCommandData, ControlState* controlState, MyoState* myoState,
    std::string applicationIdentifier, MainGUI *mainGuiHandle, ProfileManager *profileManagerHandle)
    : WearableDevice(sharedCommandData), appIdentifier(applicationIdentifier), myoFindTimeout(DEFAULT_FIND_MYO_TIMEOUT),
    durationInMilliseconds(DEFAULT_MYO_DURATION_MS), state(controlState), myoState(myoState),
    mainGui(mainGuiHandle), profileManager(profileManagerHandle), gestureFilter(controlState, myoState, 0, mainGuiHandle), emgImpulseFilter(myoState)
{
    prevProfileName = "";

    setupPosePipeline();
    setupEmgImpusePipeline();
    setupOrientationPipeline();
    setupRSSIPipeline();
}

MyoDevice::~MyoDevice()
{
    delete genAvgFilterRSSI; genAvgFilterRSSI = NULL;
    delete genWinMaxFilterRSSI; genWinMaxFilterRSSI = NULL;

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

    advancedConnectPipeline.registerFilterAtDeepestLevel(WearableDevice::sharedData);

    advancedSyncPipeline.registerFilterAtDeepestLevel(WearableDevice::sharedData);

    advancedBatteryPipeline.registerFilterAtDeepestLevel(WearableDevice::sharedData);

    profileSignaller.setControlStateHandle(state);
    if (profileManager->getProfiles()->size() > 0)
    {
        state->setProfile(profileManager->getProfiles()->at(0).profileName);
    }
    mainGui->connectSignallerToProfileWidgets(&profileSignaller);

    std::chrono::milliseconds rssi_start =
        std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()); /* Used to control when to request rssi */
    rssi_start -= std::chrono::milliseconds(MIN_RSSI_DELAY);
    std::chrono::milliseconds current_time;

    Hub* hub;
    try
    {
        Myo* myo = NULL;

        while (!myo)
        {
            try {
                hub = new Hub(appIdentifier);
                myo = hub->waitForMyo(myoFindTimeout);
            }
            catch (const std::exception& e) {
                // do nothing - keep trying!
            }

            std::cout << "Still can not find a Myo." << std::endl;
            Sleep(1000);
        }
        hub->setLockingPolicy(hub->lockingPolicyNone);

        // Enable EMG streaming on the found Myo.
        myo->setStreamEmg(myo::Myo::streamEmgEnabled);

        MyoCallbacks myoCallbacks(*this);
        hub->addListener(&myoCallbacks);

        // update battery level before main loop. Then it gets called asynchronously when the level of the Myo changes.
        myo->requestBatteryLevel();

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
                //if (myo)
                //{
                //    myo->requestRssi();
                //}
                //else
                //{
                //    myo = hub->waitForMyo(myoFindTimeout);
                //}
                rssi_start = current_time;
                if (connectedMyos.size() >= 1)
                {
                    connectedMyos[0].myo->requestRssi(); // only get signal of first connected Myo for now.
                }
            }

            hub->run(durationInMilliseconds); // this line is causing th exception on a disconnect of the only Myo

        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        WearableDevice::setDeviceStatus(deviceStatus::ERR);
        if (hub) { delete hub; hub = NULL; }
        return;
    }
    if (hub) { delete hub; hub = NULL; }

    WearableDevice::setDeviceStatus(deviceStatus::DONE);
}

void MyoDevice::setupPosePipeline()
{
    advancedPosePipeline.registerFilterAtDeepestLevel(&gestureFilter);

    advancedPosePipeline.registerFilterAtNewLevel(WearableDevice::sharedData);
}

void MyoDevice::setupOrientationPipeline()
{
    genAvgFilterQX = new GenericAveragingFilter(3, QUAT_DATA_X);
    genAvgFilterQY = new GenericAveragingFilter(3, QUAT_DATA_Y);
    genAvgFilterQZ = new GenericAveragingFilter(3, QUAT_DATA_Z);
    genAvgFilterQW = new GenericAveragingFilter(3, QUAT_DATA_W);

    genAvgFilterAY = new GenericAveragingFilter(3, ACCEL_DATA_X);
    genAvgFilterAZ = new GenericAveragingFilter(3, ACCEL_DATA_Y);
    genAvgFilterAW = new GenericAveragingFilter(3, ACCEL_DATA_Z);

    genAvgFilterGY = new GenericAveragingFilter(3, GYRO_DATA_X);
    genAvgFilterGZ = new GenericAveragingFilter(3, GYRO_DATA_Y);
    genAvgFilterGW = new GenericAveragingFilter(3, GYRO_DATA_Z);

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
    genAvgFilterRSSI = new GenericAveragingFilter(5, RSSI); // unused
    //
    //advancedRssiPipeline.registerFilterAtDeepestLevel(genAvgFilterRSSI);
    //
    //advancedRssiPipeline.registerFilterAtNewLevel(WearableDevice::sharedData);

    genWinMaxFilterRSSI = new GenericWindowMaxFilter(5, RSSI); // unused

    advancedRssiPipeline.registerFilterAtDeepestLevel(WearableDevice::sharedData);
}

void MyoDevice::setupEmgImpusePipeline()
{
    emgImpulsePipeline.registerFilterAtDeepestLevel(&emgImpulseFilter);
    emgImpulsePipeline.registerFilterAtNewLevel(WearableDevice::sharedData);
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
        Myo* myo = connectedMyos[i].myo;
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
    myoGyroDataFile.open("myoGyroData.csv");
    lastPose = Pose::rest;
}

MyoDevice::MyoCallbacks::~MyoCallbacks() { myoDataFile.close(); myoGyroDataFile.close(); }

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
    //    printToDataFile();
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
    myoGyroDataFile << gyro.x() << "," << gyro.y() << "," << gyro.z() << std::endl;

    filterDataMap input;
    input[GYRO_DATA_X] = gyro.x();
    input[GYRO_DATA_Y] = gyro.y();
    input[GYRO_DATA_Z] = gyro.z();

    parent.advancedOrientationPipeline.startPipeline(input);
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
    myo->requestBatteryLevel();
    
    std::cout << "on connect." << std::endl;
    filterDataMap input;
    input[ISCONNECTED_INPUT] = true;

    parent.advancedConnectPipeline.startPipeline(input);

    MyoWithData mwd(myo);
    mwd.fwVersion = firmwareVersion;

    if (isFWVersionHigher(firmwareVersion, highestFirmwareVersionConnected))
    {
        highestFirmwareVersionConnected = firmwareVersion;
        parent.mainGui->checkFWAndUpdate();
        parent.myoState->checkFWAndUpdate();
    }

    parent.connectedMyos.push_back(mwd);
}
void MyoDevice::MyoCallbacks::onDisconnect(Myo* myo, uint64_t timestamp) {
    std::cout << "on disconnect." << std::endl;

    for (std::vector<MyoWithData>::iterator it = parent.connectedMyos.begin(); it != parent.connectedMyos.end(); it++)
    {
        if (myo == it->myo)
        {
            parent.connectedMyos.erase(it);
            break;
        }
    }

    if (parent.connectedMyos.size() == 0)
    {
        filterDataMap input;
        input[ISCONNECTED_INPUT] = false;

        parent.advancedConnectPipeline.startPipeline(input);

        // Start each profile in a locked state
        AdvancedFilterPipeline lockPipe;
        lockPipe.registerFilterAtDeepestLevel(&(parent.gestureFilter));
        filterDataMap input2;
        input2[GESTURE_FILTER_STATE_CHANGE] = midasMode::LOCK_MODE;
        lockPipe.startPipeline(input2);

        // set battery and signal values to off
        filterDataMap input3;
        input3[BATTERY_LEVEL_INPUT] = (unsigned int) 0;
        parent.advancedBatteryPipeline.startPipeline(input3);

        filterDataMap input4;
        input4[RSSI] = -999; // extremely low db level.
        parent.advancedRssiPipeline.startPipeline(input4);
    }
}

void MyoDevice::MyoCallbacks::onArmSync(Myo *myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState)
{
    myo->requestBatteryLevel();

    parent.setMyoExtraData(myo, arm, xDirection, rotation);
    std::cout << "on arm sync." << std::endl;

    // Setup GUI to interact with most recently synched Myo
    filterDataMap input;
    input[INPUT_ARM] = arm;
    input[INPUT_X_DIRECTION] = xDirection;
    parent.advancedOrientationPipeline.startPipeline(input);

    // pass down filter pipeline for consistency, but not really used there currently
    filterDataMap syncInput;
    syncInput[SYNCHED_INPUT] = true;
    parent.advancedSyncPipeline.startPipeline(syncInput);

    // modify myoState directly - should have done this for arm/xDirection as well, but didnt for
    // legacy reasons
    parent.myoState->setXRotation(rotation);
}
void MyoDevice::MyoCallbacks::onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) {
    parent.setMyoExtraData(myo, arm, xDirection);
    std::cout << "on arm sync." << std::endl;

    // Setup GUI to interact with most recently synched Myo
    filterDataMap input;
    input[INPUT_ARM] = arm;
    input[INPUT_X_DIRECTION] = xDirection;
    parent.advancedOrientationPipeline.startPipeline(input);

    filterDataMap syncInput;
    syncInput[SYNCHED_INPUT] = true;
    parent.advancedSyncPipeline.startPipeline(syncInput);
}
void MyoDevice::MyoCallbacks::onArmUnsync(Myo* myo, uint64_t timestamp) {
    parent.setMyoExtraData(myo, Arm::armUnknown, XDirection::xDirectionUnknown);
    std::cout << "on arm unsync." << std::endl;

    filterDataMap input;
    input[SYNCHED_INPUT] = false;
    parent.advancedSyncPipeline.startPipeline(input);

    // Start each profile in a locked state
    AdvancedFilterPipeline lockPipe;
    lockPipe.registerFilterAtDeepestLevel(&(parent.gestureFilter));
    filterDataMap input2;
    input2[GESTURE_FILTER_STATE_CHANGE] = midasMode::LOCK_MODE;
    lockPipe.startPipeline(input2);
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

    filterDataMap input;
    input[BATTERY_LEVEL_INPUT] = (unsigned int)level;
    parent.advancedBatteryPipeline.startPipeline(input);
}

void MyoDevice::MyoCallbacks::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
    // This data is streaming at 200Hz - print data for post mortem analysis
    std::cout << "onEmgData." << std::endl;

    //for (int emgIdx = 0; emgIdx < 8; emgIdx++)
    //{
    //    lastEMGData[emgIdx] = emg[emgIdx];
    //}
    //printToDataFile(); // uncomment to printout data for post mortem analysis

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
    input[RSSI] = static_cast<int>(rssi);

    parent.advancedRssiPipeline.startPipeline(input);
}

void MyoDevice::updateProfiles(void)
{
    int error = (int)filterError::NO_FILTER_ERROR;

    if (profileManager->getProfiles()->size() == 0) { return; }

    error |= (int)advancedPosePipeline.updateFiltersBasedOnProfile(*profileManager, state->getProfile());

    error |= (int)advancedOrientationPipeline.updateFiltersBasedOnProfile(*profileManager, state->getProfile());

    // Start each profile in a locked state
    AdvancedFilterPipeline lockPipe;
    lockPipe.registerFilterAtDeepestLevel(&gestureFilter);
    filterDataMap input;
    input[GESTURE_FILTER_STATE_CHANGE] = midasMode::LOCK_MODE;
    lockPipe.startPipeline(input);

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
    myoDataFile << ((int)lastPose) << "," << (int)(EMGImpulseFilter::getImpulseStatus()) << std::endl;
}

void MyoDevice::setMyoExtraData(Myo* myo, Arm arm, XDirection xDirection, float rotation)
{
    int idx = 0;
    for (std::vector<MyoWithData>::iterator it = connectedMyos.begin(); it != connectedMyos.end(); it++)
    {
        if (myo == it->myo)
        {
            connectedMyos.at(idx).arm = arm;
            connectedMyos.at(idx).xDirection = xDirection;
            connectedMyos.at(idx).rotation = rotation;
            break;
        }
        idx++;
    }
    return;
}