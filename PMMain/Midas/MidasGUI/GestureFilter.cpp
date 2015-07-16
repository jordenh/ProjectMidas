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

#include "GestureFilter.h"
#include "MyoCommon.h"
#include "CommandData.h"
#include "ProfileManager.h"
#include "BaseMeasurements.h"
#include "MyoDevice.h"
#include "SharedCommandData.h"
#include "ControlState.h"
#include "GestureSeqRecorder.h"
#include "MyoState.h"
#include "MainGUI.h"
#include <time.h>
#include <thread>
#include <qtranslator.h>

ControlState* GestureFilter::controlStateHandle;
GestureSignaller GestureFilter::gestureSignaller;
SettingsSignaller GestureFilter::settingsSignaller;

GestureFilter::GestureFilter(ControlState* controlState, MyoState* myoState, clock_t timeDel, MainGUI *mainGuiHandle)
    : timeDelta(timeDel), lastPoseType(Pose::rest),
	lastTime(0), mainGui(mainGuiHandle)
{
    imageManager.loadImages();
    gestSeqRecorder = new GestureSeqRecorder(controlState, mainGuiHandle, imageManager);

    defaultMouseSequences();
    defaultKeyboardSequences();
    defaultStateSequences();

    controlStateHandle = controlState;
	myoStateHandle = myoState;

    setupCallbackThread(this);

    if (mainGui)
    {
        mainGui->connectSignallerToInfoIndicator(&gestureSignaller);
        mainGui->connectSignallerToPoseDisplayer(&gestureSignaller);
#ifdef BUILD_KEYBOARD
		mainGui->connectSignallerToKeyboardToggle(&gestureSignaller);
#endif
		mainGui->connectSignallerToProfileIcons(&gestureSignaller);
        mainGui->connectSignallerToSettingsDisplayer(&settingsSignaller);
    }

    gestureSignaller.emitStateString(QTranslator::tr((modeToString(controlState->getMode())).c_str()));

	Filter::setFilterError(filterError::NO_FILTER_ERROR);
	Filter::setFilterStatus(filterStatus::OK);
	Filter::clearOutput();
}

GestureFilter::~GestureFilter()
{
    delete gestSeqRecorder; gestSeqRecorder = NULL;
}

void GestureFilter::process()
{
    clock_t timeFromLastPose = 0;
    filterDataMap input = Filter::getInput();
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::OK);
    Filter::clearOutput();

    Pose::Type gesture;
    if (input.find(GESTURE_INPUT) != input.end())
    {
        boost::any value = input[GESTURE_INPUT];
        if (value.type() != typeid(Pose::Type))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
            return;
        }
        else
        {
            gesture = boost::any_cast<Pose::Type>(input[GESTURE_INPUT]);
        }
    }
    else
    {
        return;
    }

	// update state and GUI
	myoStateHandle->pushPose(gesture);
    emitPoseData(gesture);

	if (gesture != lastPoseType)
	{
        if (lastPoseType != Pose::rest && gesture != Pose::rest)
        {
            // Should not ever happen! (taken care of in MyoDevice)
            // But for now, inject a rest pose since Midas expects a rest inbetween all other poses
            gesture = Pose::rest;
        }

		BaseMeasurements::getInstance().setCurrentPose(gesture);
		
        // Update screen size every time there is a new pose, as to ensure it is up to date 
        // for possible mouse calculations.
		BaseMeasurements::getInstance().updateScreenSize();
	}

    CommandData response;
    SequenceStatus ss;
    ss = gestSeqRecorder->progressSequence(gesture, *controlStateHandle, response);
    
    if (response.type == commandType::NONE)
    {
        // terminate filter pipeline, as nothing to add
        Filter::setFilterStatus(filterStatus::END_CHAIN);
    }

    if (response.type == commandType::STATE_CHANGE)
    {
        handleStateChange(response, this);
    }
    else if (response.type == commandType::MOUSE_CMD)
    {
        handleMouseCommand(response);
    }
    else if (response.type == commandType::KYBRD_CMD || response.type == commandType::KYBRD_GUI_CMD)
    {
        handleKybrdCommand(response);
    }
	else if (response.type == commandType::PROFILE_CHANGE)
	{
		handleProfileChangeCommand(response);
	}

    lastPoseType = gesture;
    lastTime = clock();
}

void GestureFilter::emitPoseData(int poseInt)
{
    std::vector<int> ids;
    std::vector<PoseLength> lengths;
    ids.push_back(poseInt);
    lengths.push_back(PoseLength::TAP); // use Tap since pose data shouldnt have an overlay

    std::vector<sequenceImageSet> images = imageManager.formSequenceSetFromIds(ids, lengths);

    if (images.size() == 1)
    {
        gestureSignaller.emitPoseImages(images);
    }
}

void GestureFilter::defaultMouseSequences(void)
{
    // Register sequence to left click in mouse mode and gesture mode
    sequence clickSeq;
    clickSeq.push_back(SeqElement(MYO_GESTURE_LEFT_MOUSE, PoseLength::IMMEDIATE));
    CommandData clickResp;
    clickResp.name = "Left Click";
    clickResp.type = commandType::MOUSE_CMD;
    clickResp.action.mouse = mouseCmds::LEFT_HOLD;
    int ss = (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Left Click");
    // setup clicking in gesture Mode
    clickResp.action.mouse = mouseCmds::LEFT_CLICK;
    clickSeq.at(0).poseLen = PoseLength::TAP; 
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp, "Left Click");

    // Register sequence to right click in mouse mode and gesture mode
    clickSeq.clear();
    clickSeq.push_back(SeqElement(MYO_GESTURE_RIGHT_MOUSE, PoseLength::IMMEDIATE));
    clickResp.name = "Right Click";
    clickResp.type = commandType::MOUSE_CMD;
    clickResp.action.mouse = mouseCmds::RIGHT_HOLD;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Right Click");
    // setup clicking in gesture Mode
    clickResp.action.mouse = mouseCmds::RIGHT_CLICK;
    clickSeq.at(0).poseLen = PoseLength::TAP;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp, "Right Click");

    // allow clicking and dragging of any button by releasing mouse buttons on rest (immediate still).
    clickResp.action.mouse = mouseCmds::RELEASE_LRM_BUTS;
    clickResp.name = "Release Mouse";
	clickSeq.clear();
    clickSeq.push_back(SeqElement(Pose::rest, PoseLength::IMMEDIATE));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Release Mouse");

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException: mouse");
    }
}

void GestureFilter::defaultKeyboardSequences(void)
{
    sequence kybrdGUISequence;
    CommandData kybrdGUIResponse;

    // register arm specific commands first
    kybrdGUIResponse.name = "Swap Ring Focus";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::SWAP_RING_FOCUS;
    if (this->myoStateHandle->getArm() == Arm::armLeft)
    {
        kybrdGUISequence.push_back(SeqElement(Pose::Type::waveIn));
    }
    else
    {
        kybrdGUISequence.push_back(SeqElement(Pose::Type::waveOut));
    }
    int ss = (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Swap Ring Focus");
    kybrdGUIResponse.name = "Backspace";
    kybrdGUIResponse.type = commandType::KYBRD_CMD;
    kybrdGUIResponse.action.kybd = kybdCmds::BACKSPACE;
    if (this->myoStateHandle->getArm() == Arm::armLeft)
    {
        kybrdGUISequence[0] = (SeqElement(Pose::Type::waveOut));
    }
    else
    {
        kybrdGUISequence[0] = (SeqElement(Pose::Type::waveIn));
    }
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Backspace");

    // register the rest of the keyboard commands
    kybrdGUIResponse.name = "Select";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::SELECT;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Select");

    kybrdGUIResponse.name = "Hold Select";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::HOLD_SELECT;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist, PoseLength::HOLD));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Hold Select");

    kybrdGUIResponse.name = "Change Wheels";
    kybrdGUIResponse.type = commandType::KYBRD_GUI_CMD;
    kybrdGUIResponse.action.kybdGUI = kybdGUICmds::CHANGE_WHEELS;
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fingersSpread));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Change Wheels");

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException: keyboard");
    }
}

void GestureFilter::defaultStateSequences(void)
{
    // Register sequence from lock to Mouse Mode
    sequence lockToMouseSeq;
    lockToMouseSeq.push_back(SeqElement(Pose::Type::doubleTap));
    lockToMouseSeq.push_back(SeqElement(Pose::Type::waveIn));
    lockToMouseSeq.push_back(SeqElement(Pose::Type::waveOut));
    CommandData lockToMouseResponse;
    lockToMouseResponse.name = "Unlock";
    lockToMouseResponse.type = commandType::STATE_CHANGE;
    lockToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    int ss = (int)gestSeqRecorder->registerSequence(midasMode::LOCK_MODE, lockToMouseSeq, lockToMouseResponse, "Unlock");

    // Register sequence from Mouse Mode to Gesture Mode
    sequence mouseToGestSeq;
    mouseToGestSeq.push_back(SeqElement(Pose::Type::doubleTap));
    CommandData mouseToGestResponse;
    mouseToGestResponse.name = "Mouse To Gesture";
    mouseToGestResponse.type = commandType::STATE_CHANGE;
    mouseToGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, mouseToGestSeq, mouseToGestResponse, "Mouse to Gesture");

    // Register sequence from Mouse Mode to Keyboard Mode
    sequence mouseToKybrdSeq;
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveOut));
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveIn));
    CommandData mouseToKybrdResponse;
    mouseToKybrdResponse.name = "Mouse To Keyboard";
    mouseToKybrdResponse.type = commandType::STATE_CHANGE;
    mouseToKybrdResponse.action.mode = midasMode::KEYBOARD_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, mouseToKybrdSeq, mouseToKybrdResponse, "Mouse to Keyboard");

    // Register sequence from Gesture Mode to Mouse Mode
    sequence gestureToMouseSeq;
    gestureToMouseSeq.push_back(SeqElement(Pose::Type::doubleTap));
    CommandData gestureToMouseResponse;
    gestureToMouseResponse.name = "Gesture To Mouse";
    gestureToMouseResponse.type = commandType::STATE_CHANGE;
    gestureToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, gestureToMouseSeq, gestureToMouseResponse, "Gesture to Mouse");

    // Register sequence from Keyboard Mode to Mouse Mode
    sequence kybrdToMouseSeq;
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::doubleTap));
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::waveOut));
    CommandData kybrdToMouseResponse;
    kybrdToMouseResponse.name = "Keyboard To Mouse";
    kybrdToMouseResponse.type = commandType::STATE_CHANGE;
    kybrdToMouseResponse.action.mode = midasMode::MOUSE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdToMouseSeq, kybrdToMouseResponse, "Keyboard to Mouse");

    // Register sequence from most Modes to Lock Mode.
    sequence toLockSeq;
    toLockSeq.push_back(SeqElement(Pose::Type::waveIn));
    toLockSeq.push_back(SeqElement(Pose::Type::doubleTap));
    CommandData toLockResponse;
    toLockResponse.type = commandType::STATE_CHANGE;
    toLockResponse.action.mode = midasMode::LOCK_MODE;
    // From Mouse:
    toLockResponse.name = "Mouse To Lock";
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, toLockSeq, toLockResponse, "Mouse to Lock");
    // From Gesture:
    toLockResponse.name = "Gesture To Lock";
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toLockSeq, toLockResponse, "Gesture to Lock");
    // From Keyboard:
    toLockResponse.name = "Keyboard To Lock"; // OVERRIDE for Keyboard.
    toLockSeq[0] = (SeqElement(Pose::Type::doubleTap));
    toLockSeq[1] = (SeqElement(Pose::Type::waveIn));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, toLockSeq, toLockResponse, "Keyboard to Lock");

    // Register sequence from Gesture Mode to Gesture Hold Modes
    sequence toHoldGestSeq;
    toHoldGestSeq.push_back(SeqElement(Pose::Type::doubleTap, PoseLength::HOLD));
    CommandData toHoldGestResponse;
    toHoldGestResponse.name = "Gesture to Hold Gesture X";
    toHoldGestResponse.type = commandType::STATE_CHANGE;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_ONE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Double Tap - not working");

    toHoldGestSeq[0].type = Pose::Type::fingersSpread;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_TWO;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Fingers Spread");

    toHoldGestSeq[0].type = Pose::Type::fist;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_THREE;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Fist");

    toHoldGestSeq[0].type = Pose::Type::waveIn;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FOUR;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Wave In");

    toHoldGestSeq[0].type = Pose::Type::waveOut;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FIVE;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Wave Out");

    // Register sequences back to Gesture Mode from Gesture Hold Modes

    sequence fromHoldGestSeq;
    fromHoldGestSeq.push_back(SeqElement(Pose::Type::rest, PoseLength::IMMEDIATE));
    CommandData fromHoldGestResponse;
    fromHoldGestResponse.name = "Gesture from Hold Gesture X";
    fromHoldGestResponse.type = commandType::STATE_CHANGE;
    fromHoldGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_ONE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Double Tap - Not working.");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_TWO, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Fingers Spread");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_THREE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Fist");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FOUR, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Wave In");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FIVE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Wave Out");

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }
}

void GestureFilter::handleStateChange(CommandData response, GestureFilter *gf)
{
    buzzFeedbackMode bfm = settingsSignaller.getBuzzFeedbackMode();
    if (bfm >= buzzFeedbackMode::MINIMAL)
    {
        if (gf->controlStateHandle->getMode() == LOCK_MODE || response.action.mode == LOCK_MODE)
        {
            gf->myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationMedium);
        }
        else if (bfm >= buzzFeedbackMode::ALLSTATECHANGES)
        {
            gf->myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationShort);
        }
    }

    if (response.type != commandType::STATE_CHANGE)
    {
        // Should NOT have made it here
        return;
    }

    if (response.action.mode == midasMode::KEYBOARD_MODE || controlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        gestureSignaller.emitToggleKeyboard();
    }

    gestureSignaller.emitStateString(QTranslator::tr((modeToString(response.action.mode)).c_str()));

    std::cout << "Transitioning to state: " << response.action.mode << std::endl;
    controlStateHandle->setMode(response.action.mode);

    // Handle any commands that may be associated with a stateChange as that is allowed in the specs

	// If there are subsequent commands to execute, do so on a seperate pipeline! -- Note this assumes no further 
	// filtering is desired on this data, and it can go straight to the SCD
	std::vector<CommandData> changeStateCommands = response.getChangeStateActions();
	AdvancedFilterPipeline fp;
    // store and replace filterDataMap incase it's used.
    filterDataMap init_fdm = gf->getOutput();
	fp.registerFilterAtDeepestLevel(gf->controlStateHandle->getSCD());
	for (int i = 0; i < changeStateCommands.size(); i++)
	{
		filterDataMap dataMap;
		if (changeStateCommands[i].type == commandType::MOUSE_CMD)
		{
			dataMap = gf->handleMouseCommand(changeStateCommands[i]);
		}
		else if (changeStateCommands[i].type == commandType::KYBRD_CMD || changeStateCommands[i].type == commandType::KYBRD_GUI_CMD)
		{
			dataMap = gf->handleKybrdCommand(changeStateCommands[i]);
        }
        else {
            continue;
        }
        gf->clearOutput();
		fp.startPipeline(dataMap);
	}
    gf->setOutput(init_fdm);
	    
    return;
}

filterDataMap GestureFilter::handleMouseCommand(CommandData response)
{
	filterDataMap outputToSharedCommandData;

    CommandData command;
    command = response;

    outputToSharedCommandData[COMMAND_INPUT] = command;
    Filter::setOutput(outputToSharedCommandData);

    buzzFeedbackMode bfm = settingsSignaller.getBuzzFeedbackMode();
    if (bfm >= buzzFeedbackMode::ALLACTIONS)
    {
        myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationShort);
    }
	return outputToSharedCommandData;
}

filterDataMap GestureFilter::handleKybrdCommand(CommandData response, bool addToExtra)
{
    filterDataMap outputToSharedCommandData;
    CommandData command;
    command = response;

    outputToSharedCommandData[COMMAND_INPUT] = command;

    if (addToExtra) 
    {
        extraDataForSCD = outputToSharedCommandData;
    }
    else
    {
        Filter::setOutput(outputToSharedCommandData);

        buzzFeedbackMode bfm = settingsSignaller.getBuzzFeedbackMode();
        if (bfm >= buzzFeedbackMode::ALLACTIONS)
        {
            myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationShort);
        }
    }

	return outputToSharedCommandData;
}

filterDataMap GestureFilter::handleProfileChangeCommand(CommandData response)
{
	filterDataMap outputToSharedCommandData;
	CommandData command;
	command = response;

    buzzFeedbackMode bfm = settingsSignaller.getBuzzFeedbackMode();
    if (bfm >= buzzFeedbackMode::MINIMAL)
    {
        myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationShort, 2);
    }

    if (response.action.profile == MOVE_PROFILE_FORWARD)
    {
        gestureSignaller.emitProfileChange(true);
    }
    else if (response.action.profile == MOVE_PROFILE_BACKWARD)
    {
        gestureSignaller.emitProfileChange(false);
    }

	outputToSharedCommandData[COMMAND_INPUT] = command;
	setOutput(outputToSharedCommandData);
	setFilterError(filterError::NO_FILTER_ERROR);
	setFilterStatus(filterStatus::OK);
	return outputToSharedCommandData;
}

void GestureFilter::handleProfileChangeCommand(CommandData response, GestureFilter *gf)
{
	// If there are subsequent commands to execute, do so on a seperate pipeline! -- Note this assumes no further 
	// filtering is desired on this data, and it can go straight to the SCD
	AdvancedFilterPipeline fp;
    filterDataMap init_fdm = gf->getOutput();
	fp.registerFilterAtDeepestLevel(gf->controlStateHandle->getSCD());
	filterDataMap dataMap;
	dataMap = gf->handleProfileChangeCommand(response);
	fp.startPipeline(dataMap);

    // revert output to init status.
    gf->setOutput(init_fdm);
}

filterDataMap GestureFilter::getExtraDataForSCD()
{
    filterDataMap retVal = extraDataForSCD;
    extraDataForSCD.clear();
    return retVal;
}

void setupCallbackThread(GestureFilter *gf)
{
    std::thread callbackThread(callbackThreadWrapper, gf);
    callbackThread.detach();
}

void callbackThreadWrapper(GestureFilter *gf)
{
    std::chrono::milliseconds period(SLEEP_LEN);
    do {
        std::this_thread::sleep_for(period);
        if (gf->getGestureSeqRecorder() == NULL)
        {
            continue;// 
        }
        gf->getGestureSeqRecorder()->checkProgressBaseTime();

        CommandData response;
        gf->getGestureSeqRecorder()->progressSequenceTime(SLEEP_LEN, response);
        if (response.type == commandType::STATE_CHANGE)
        {
            GestureFilter::handleStateChange(response, gf);
        }
        else if (response.type == commandType::MOUSE_CMD)
        {
            gf->handleMouseCommand(response);
        }
        else if (response.type == commandType::KYBRD_CMD)
        {
            gf->handleKybrdCommand(response, true);
        }
        else if (response.type == commandType::KYBRD_GUI_CMD)
        {
            gf->handleKybrdCommand(response, true);
        }
		else if (response.type == commandType::PROFILE_CHANGE)
		{
			//gf->handleProfileChangeCommand(response);
			GestureFilter::handleProfileChangeCommand(response, gf);

		}
    } while (true);
}

filterError GestureFilter::updateBasedOnProfile(ProfileManager& pm, std::string name)
{
    gestSeqRecorder->unregisterAll();

    std::vector<profile>* profiles = pm.getProfiles();

    profile prof;
    bool foundProfile = false;
    for (int i = 0; i < profiles->size(); i++)
    {
        if (name == profiles->at(i).profileName)
        {
            prof = profiles->at(i);
            foundProfile = true;
        }
    }

    if (!foundProfile) return filterError::PROCESSING_ERROR;

    int ss = (int)SequenceStatus::SUCCESS;
    for (std::vector<profileSequence>::iterator it = prof.profileSequences.begin(); it != prof.profileSequences.end(); ++it)
    {
        sequence seq;
        for (std::vector<gesture>::iterator gestureIt = it->gestures.begin(); gestureIt != it->gestures.end(); ++gestureIt)
        {
            PoseLength len = profileGestureTypeToPoseLength[gestureIt->type];
            Pose::Type type = profileGestureNameToType[gestureIt->name];
            seq.push_back(SeqElement(type, len));
        }

		// For now, only ONE command can be used for all command types, except state changes.
		// On a state change, the user can have Midas send a vector of commands as soon
		// as they change into the state.
		std::vector<CommandData> translatedCommands;
		int cmdItCount = 0;
		for (std::vector<command>::iterator cmdIt = it->cmds.begin(); cmdIt != it->cmds.end(); ++cmdIt)
		{
			CommandData translatedCommand;
			translatedCommand.name = it->name;
			translatedCommand.type = profileCommandToCommandTypeMap[cmdIt->type]; 
			// Currently only supporting one action, rather than a list.
			// The XML format supports a list so that it can be extended in Midas easily.
			std::string action = cmdIt->actions[0];
			switch (translatedCommand.type)
			{
			case commandType::KYBRD_CMD:
				if (action.find("inputVector") == 0)
				{
					// special case where user could specify 0 or more keys to be pressed
					// in the format: "inputVector,ABCD..." where A, B, C, D... are all keys
					// intended to be added to the keyboardVector response.
					translatedCommand.action.kybd = profileActionToKybd["inputVector"];
					translatedCommand.keyboardVector = KeyboardVector::createFromProfileStr(action);
				}
				else
				{
					translatedCommand.action.kybd = profileActionToKybd[action];
				}
				break;
			case commandType::KYBRD_GUI_CMD:
				if (profileActionToKybdGui.find(action) != profileActionToKybdGui.end())
				{
					translatedCommand.action.kybdGUI = profileActionToKybdGui[action];
				}
				else
				{
					translatedCommand.type = commandType::KYBRD_CMD;
					translatedCommand.action.kybd = profileActionToKybd[action];
				}
				break;
			case commandType::MOUSE_CMD:
				translatedCommand.action.mouse = profileActionToMouseCommands[action];
				break;
			case commandType::STATE_CHANGE:
				if (cmdItCount > 0)
				{
					throw new std::exception("GestureFilter: StateChangeAction set as StateChange");
				}
				translatedCommand.action.mode = profileActionToStateChange[action];
				break;
			case commandType::PROFILE_CHANGE:
				translatedCommand.action.profile = profileActionToProfileChange[action];
			default:
				break;
			}
			translatedCommands.push_back(translatedCommand);
			cmdItCount++;
		}

		CommandData response;
		response = translatedCommands[0];
		// if more than one command, add as changeStateActions
		for (int i = 1; i < cmdItCount; i++)
		{
			response.addChangeStateAction(translatedCommands[i]);
		}

        midasMode startState = profileActionToStateChange[it->state];
        ss |= (int)gestSeqRecorder->registerSequence(startState, seq, response, it->name);
    }

#ifdef BUILD_WITH_HOLD_MODES
    // Register sequence from Gesture Mode to Gesture Hold Modes
    sequence toHoldGestSeq;
    toHoldGestSeq.push_back(SeqElement(Pose::Type::doubleTap, PoseLength::HOLD));
    CommandData toHoldGestResponse;
    toHoldGestResponse.name = "Gesture to Hold Gesture X";
    toHoldGestResponse.type = commandType::STATE_CHANGE;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_ONE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Double Tap - not working");

    toHoldGestSeq[0].type = Pose::Type::fingersSpread;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_TWO;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Fingers Spread");

    toHoldGestSeq[0].type = Pose::Type::fist;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_THREE;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Fist");

    toHoldGestSeq[0].type = Pose::Type::waveIn;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FOUR;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Wave In");

    toHoldGestSeq[0].type = Pose::Type::waveOut;
    toHoldGestResponse.action.mode = midasMode::GESTURE_HOLD_FIVE;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toHoldGestSeq, toHoldGestResponse, "Gesture to Hold Wave Out");

    sequence fromHoldGestSeq;
    fromHoldGestSeq.push_back(SeqElement(Pose::Type::rest, PoseLength::IMMEDIATE));
    CommandData fromHoldGestResponse;
    fromHoldGestResponse.name = "Gesture from Hold Gesture X";
    fromHoldGestResponse.type = commandType::STATE_CHANGE;
    fromHoldGestResponse.action.mode = midasMode::GESTURE_MODE;

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_ONE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Double Tap - not working"); // TODO handle these..
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_TWO, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Fingers Spread");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_THREE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Fist");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FOUR, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Wave In");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FIVE, fromHoldGestSeq, fromHoldGestResponse, "Gesture from Hold Wave Out");
#endif

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }

    gestSeqRecorder->updateGuiSequences();
    return filterError::NO_FILTER_ERROR;
}