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

#pragma once
#include "WearableDevice.h"
#include "GestureFilter.h"
#include "EMGImpulseFilter.h"
#include "AdvancedFilterPipeline.h"
#include "myo\myo.hpp"
#include "ProfileSignaller.h"

#include <iostream>
#include <fstream>

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

#define DEFAULT_FIND_MYO_TIMEOUT 1000
#define DEFAULT_MYO_DURATION_MS 1000/20
#define DEFAULT_MYO_ARM Arm::armUnknown
#define DEFAULT_MYO_XDIR XDirection::xDirectionUnknown

// The xRotationMatrix will contain a matrix that when multiplied by a 
// vector, will rotate it TOWARDS the desired location. This will allow
// for accelerated cursor movement to occur in the proper direction.
#define DEFAULT_DESIRED_X_ROTATION_FW_1_4_1670 M_PI
#define DEFAULT_DESIRED_X_ROTATION_FW_1_5_1931 2.120



class ControlState;
class MyoState;
class MainGUI;
class ProfileManager;
class GenericAveragingFilter;
class GenericWindowMaxFilter;
class GenericBypassFilter;
class MyoTranslationFilter;

/**
 * Handles the Myo device, collecting the data using the Myo API, and converting the data
 * to keyboard and mouse commands by sending it through a data pipeline.
 */
class MyoDevice : public WearableDevice
{
public:
    /**
     * The constructor for the MyoDevice.
     *
     * @param sharedCommandData A handle to the SCD so that the device can post commands.
     * @param controlState A handle to the ControlState object so that the application state can be 
     * changed, and so that the device can read the current state.
     * @param applicationIdentifier A myo-specific app identifier used to create the myo hub.
     */
	MyoDevice(SharedCommandData* sharedCommandData, ControlState* controlState, MyoState* myoState, std::string applicationIdentifier,
		MainGUI *mainGuiHandle, ProfileManager* profileManagerHandle);
    ~MyoDevice();

    /**
     * Sets the amount of time that should be spent looking for a Myo device. If a Myo
     * cannot be found within this amount of time, the device thread will exit with an
     * error.
     *
     * @param milliseconds Number of milliseconds to wait for a Myo. If this is a 0, it will
     * search until one is found (indefinitely).
     */
    void setFindMyoTimeout(unsigned int milliseconds);

    /**
     * Sets the amount of time, in milliseconds, the Myo event loop should run. While the Myo
     * event loop is running, Midas has no control over the device. The device can only be exited while
     * outside of the Myo event loop.
     *
     * @param milliseconds Number of milliseconds to stay in the event loop.
     */
    void setMyoEventLoopDuration(unsigned int milliseconds);

    // Overridden functions
    /**
     * Runs the device loop. This sets up the data pipeline, searches for a Myo device, registers
     * the callback functions, and then enters a loop that waits for events and calls the callback
     * functions.
     */
    void runDeviceLoop();

    /**
     * Returns an error code associated with the device, once the device thread has stopped running.
     *
     * @return An error code.
     */
    int getDeviceError();

    /**
     * Update filters internal mechanisms if the profile changes.
     *
     * @return void.
     */
    void updateProfiles(void);

    /**
     * Send a vibration command to the connected myos.
     *
     * @param vibType - indicates Myo vibration type (currently short, medium, or long)
     */
    void vibrateMyos(myo::Myo::VibrationType vibType, int numReps = 1) const;

    static FirmwareVersion getHighestFirmwareVersionConnected() { return highestFirmwareVersionConnected; }

private:
    void setupPosePipeline();
    void setupOrientationPipeline();
    void setupRSSIPipeline();
    void setupEmgImpusePipeline();

    void setMyoExtraData(Myo* myo, Arm arm, XDirection xDirection, float rotation = 0.0f);

    /**
     * This class implements all of the callback functions from the Myo DeviceListener
     * class. The methods in this class are called when Myo events occur.
     */
    class MyoCallbacks : public DeviceListener
    {
    public:
        MyoCallbacks(MyoDevice& parentDevice);
        ~MyoCallbacks();
        
        // Overridden functions from DeviceListener
        void onPair(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion);
        void onUnpair(Myo* myo, uint64_t timestamp);
        void onConnect(Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion);
        void onDisconnect(Myo* myo, uint64_t timestamp);
        void onArmSync(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection);
        void onArmSync(Myo *myo, uint64_t timestamp, Arm arm, XDirection xDirection, float rotation, WarmupState warmupState); // SDK V0.9.0
        void onArmUnsync(Myo* myo, uint64_t timestamp);
        // For SDK <= 5, use these 2 arm callbacks.
        void onArmRecognized(Myo* myo, uint64_t timestamp, Arm arm, XDirection xDirection) { onArmSync(myo, timestamp, arm, xDirection); }
        void onArmLost(Myo* myo, uint64_t timestamp) { onArmUnsync(myo, timestamp); }
        void onPose(Myo* myo, uint64_t timestamp, Pose pose);
        void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& rotation);
        void onAccelerometerData(Myo* myo, uint64_t timestamp, const Vector3<float>& accel);
        void onGyroscopeData(Myo* myo, uint64_t timestamp, const Vector3<float>& gyro);
        void onRssi(Myo* myo, uint64_t timestamp, int8_t rssi);
		// Added on upgrade to SDK Win 0.9.0
		void onUnlock(Myo* myo, uint64_t timestamp);
		void onLock(Myo* myo, uint64_t timestamp);
		void onBatteryLevelReceived(myo::Myo* myo, uint64_t timestamp, uint8_t level);
		void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
		void onWarmupCompleted(myo::Myo* myo, uint64_t timestamp, WarmupResult warmupResult);

    private:
        MyoDevice& parent;

        void printToDataFile();
        std::ofstream myoDataFile;
        std::ofstream myoGyroDataFile;
        Pose::Type lastPose;
        int8_t lastEMGData[8];
    };

    struct MyoWithData {
        MyoWithData(Myo* myo) 
        {   
            this->myo = myo; arm = Arm::armUnknown; xDirection = XDirection::xDirectionUnknown; rotation = 0; 
            fwVersion.firmwareVersionMajor = 0; fwVersion.firmwareVersionMinor = 0; fwVersion.firmwareVersionPatch = 0; fwVersion.firmwareVersionHardwareRev = 0;
        }

        Myo* myo;
        Arm arm;
        XDirection xDirection;
        float rotation;
        FirmwareVersion fwVersion;
    };

    std::vector<MyoWithData> connectedMyos; // not owned
        unsigned int myoFindTimeout;
    unsigned int durationInMilliseconds;
    std::string appIdentifier;

    ControlState* state; // not owned
    MyoState* myoState; // not owned
    AdvancedFilterPipeline advancedPosePipeline, advancedOrientationPipeline,
        advancedRssiPipeline, advancedConnectPipeline, advancedSyncPipeline, emgImpulsePipeline,
        advancedBatteryPipeline;
    MainGUI *mainGui; // not owned

    std::string prevProfileName;

    static ProfileSignaller profileSignaller;
    ProfileManager *profileManager; // not owned

    GestureFilter gestureFilter;
    EMGImpulseFilter emgImpulseFilter;
    // owned filters
    GenericAveragingFilter *genAvgFilterRSSI;
    GenericWindowMaxFilter *genWinMaxFilterRSSI;

    GenericAveragingFilter *genAvgFilterQX;
    GenericAveragingFilter *genAvgFilterQY;
    GenericAveragingFilter *genAvgFilterQZ;
    GenericAveragingFilter *genAvgFilterQW;

    GenericAveragingFilter *genAvgFilterAY;
    GenericAveragingFilter *genAvgFilterAZ;
    GenericAveragingFilter *genAvgFilterAW;

    GenericAveragingFilter *genAvgFilterGY;
    GenericAveragingFilter *genAvgFilterGZ;
    GenericAveragingFilter *genAvgFilterGW;

    GenericBypassFilter *genBypassFilterArm;
    GenericBypassFilter *genBypassFilterXDir;

    MyoTranslationFilter *translationFilter;

    static FirmwareVersion highestFirmwareVersionConnected;
};

