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
    controlStateHandle = controlState;
    myoStateHandle = myoState;

    imageManager.loadImages();
    gestSeqRecorder = new GestureSeqRecorder(controlState, mainGuiHandle, imageManager);

    defaultMouseSequences();
    defaultKeyboardSequences();
    defaultStateSequences();

    setupCallbackThread(this);

    if (mainGui)
    {
        mainGui->connectSignallerToInfoIndicator(&gestureSignaller);
        mainGui->connectSignallerToPoseDisplayer(&gestureSignaller);
		mainGui->connectSignallerToKeyboardToggle(&gestureSignaller);
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
    else if (input.find(GESTURE_FILTER_STATE_CHANGE) != input.end())
    {
        boost::any value = input[GESTURE_FILTER_STATE_CHANGE];
        if (value.type() != typeid(midasMode))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
            return;
        }
        else
        {
            midasMode forceState = boost::any_cast<midasMode>(input[GESTURE_FILTER_STATE_CHANGE]);
            CommandData forceStateCmd;
            forceStateCmd.setType(commandType::STATE_CHANGE);
            forceStateCmd.setActionMode(forceState);
            forceStateCmd.setName("Force State Change");
            handleStateChange(forceStateCmd, this);
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
    
    if (response.getType() == commandType::NONE)
    {
        // terminate filter pipeline, as nothing to add
        Filter::setFilterStatus(filterStatus::END_CHAIN);
    }

    if (response.getType() == commandType::STATE_CHANGE)
    {
        handleStateChange(response, this);
    }
    else if (response.getType() == commandType::MOUSE_CMD)
    {
        handleMouseCommand(response);
    }
    else if (response.getType() == commandType::KYBRD_CMD || response.getType() == commandType::KYBRD_GUI_CMD)
    {
        handleKybrdCommand(response);
    }
	else if (response.getType() == commandType::PROFILE_CHANGE)
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
    clickResp.setName("Left Click");
    clickResp.setType(commandType::MOUSE_CMD);
    clickResp.setActionMouse(mouseCmds::LEFT_HOLD);
    int ss = (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Left Click");
    // setup clicking in gesture Mode
    clickResp.setActionMouse(mouseCmds::LEFT_CLICK);
    clickSeq.at(0).poseLen = PoseLength::TAP; 
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp, "Left Click");

    // Register sequence to right click in mouse mode and gesture mode
    clickSeq.clear();
    clickSeq.push_back(SeqElement(MYO_GESTURE_RIGHT_MOUSE, PoseLength::IMMEDIATE));
    clickResp.setName("Right Click");
    clickResp.setType(commandType::MOUSE_CMD);
    clickResp.setActionMouse(mouseCmds::RIGHT_HOLD);
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, clickSeq, clickResp, "Right Click");
    // setup clicking in gesture Mode
    clickResp.setActionMouse(mouseCmds::RIGHT_CLICK);
    clickSeq.at(0).poseLen = PoseLength::TAP;
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, clickSeq, clickResp, "Right Click");

    // allow clicking and dragging of any button by releasing mouse buttons on rest (immediate still).
    clickResp.setActionMouse(mouseCmds::RELEASE_LRM_BUTS);
    clickResp.setName("Release Mouse");
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
    kybrdGUIResponse.setName("Swap Ring Focus");
    kybrdGUIResponse.setType(commandType::KYBRD_GUI_CMD);
    kybrdGUIResponse.setActionKybdGUI(kybdGUICmds::SWAP_RING_FOCUS);
    if (myoStateHandle->getArm() == Arm::armLeft)
    {
        kybrdGUISequence.push_back(SeqElement(Pose::Type::waveIn));
    }
    else
    {
        kybrdGUISequence.push_back(SeqElement(Pose::Type::waveOut));
    }
    int ss = (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Swap Ring Focus");
    kybrdGUIResponse.setName("Backspace");
    kybrdGUIResponse.setType(commandType::KYBRD_CMD);
    kybrdGUIResponse.setActionKybd(kybdCmds::BACKSPACE);
    if (myoStateHandle->getArm() == Arm::armLeft)
    {
        kybrdGUISequence[0] = (SeqElement(Pose::Type::waveOut));
    }
    else
    {
        kybrdGUISequence[0] = (SeqElement(Pose::Type::waveIn));
    }
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Backspace");

    // register the rest of the keyboard commands
    kybrdGUIResponse.setName("Select");
    kybrdGUIResponse.setType(commandType::KYBRD_GUI_CMD);
    kybrdGUIResponse.setActionKybdGUI(kybdGUICmds::SELECT);
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Select");

    kybrdGUIResponse.setName("Hold Select");
    kybrdGUIResponse.setType(commandType::KYBRD_GUI_CMD);
    kybrdGUIResponse.setActionKybdGUI(kybdGUICmds::HOLD_SELECT);
    kybrdGUISequence[0] = (SeqElement(Pose::Type::fist, PoseLength::HOLD));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdGUISequence, kybrdGUIResponse, "Hold Select");

    kybrdGUIResponse.setName("Change Wheels");
    kybrdGUIResponse.setType(commandType::KYBRD_GUI_CMD);
    kybrdGUIResponse.setActionKybdGUI(kybdGUICmds::CHANGE_WHEELS);
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
    lockToMouseResponse.setName("Unlock");
    lockToMouseResponse.setType(commandType::STATE_CHANGE);
    lockToMouseResponse.setActionMode(midasMode::MOUSE_MODE);

    int ss = (int)gestSeqRecorder->registerSequence(midasMode::LOCK_MODE, lockToMouseSeq, lockToMouseResponse, "Unlock");

    // Register sequence from Mouse Mode to Gesture Mode
    sequence mouseToGestSeq;
    mouseToGestSeq.push_back(SeqElement(Pose::Type::doubleTap));
    CommandData mouseToGestResponse;
    mouseToGestResponse.setName("Mouse To Gesture");
    mouseToGestResponse.setType(commandType::STATE_CHANGE);
    mouseToGestResponse.setActionMode(midasMode::GESTURE_MODE);

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, mouseToGestSeq, mouseToGestResponse, "Mouse to Gesture");

    // Register sequence from Mouse Mode to Keyboard Mode
    sequence mouseToKybrdSeq;
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveOut));
    mouseToKybrdSeq.push_back(SeqElement(Pose::Type::waveIn));
    CommandData mouseToKybrdResponse;
    mouseToKybrdResponse.setName("Mouse To Keyboard");
    mouseToKybrdResponse.setType(commandType::STATE_CHANGE);
    mouseToKybrdResponse.setActionMode(midasMode::KEYBOARD_MODE);

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, mouseToKybrdSeq, mouseToKybrdResponse, "Mouse to Keyboard");

    // Register sequence from Gesture Mode to Mouse Mode
    sequence gestureToMouseSeq;
    gestureToMouseSeq.push_back(SeqElement(Pose::Type::doubleTap));
    CommandData gestureToMouseResponse;
    gestureToMouseResponse.setName("Gesture To Mouse");
    gestureToMouseResponse.setType(commandType::STATE_CHANGE);
    gestureToMouseResponse.setActionMode(midasMode::MOUSE_MODE);

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, gestureToMouseSeq, gestureToMouseResponse, "Gesture to Mouse");

    // Register sequence from Keyboard Mode to Mouse Mode
    sequence kybrdToMouseSeq;
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::doubleTap));
    kybrdToMouseSeq.push_back(SeqElement(Pose::Type::waveOut));
    CommandData kybrdToMouseResponse;
    kybrdToMouseResponse.setName("Keyboard To Mouse");
    kybrdToMouseResponse.setType(commandType::STATE_CHANGE);
    kybrdToMouseResponse.setActionMode(midasMode::MOUSE_MODE);

    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, kybrdToMouseSeq, kybrdToMouseResponse, "Keyboard to Mouse");

    // Register sequence from most Modes to Lock Mode.
    sequence toLockSeq;
    toLockSeq.push_back(SeqElement(Pose::Type::waveIn));
    toLockSeq.push_back(SeqElement(Pose::Type::doubleTap));
    CommandData toLockResponse;
    toLockResponse.setType(commandType::STATE_CHANGE);
    toLockResponse.setActionMode(midasMode::LOCK_MODE);
    // From Mouse:
    toLockResponse.setName("Mouse To Lock");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::MOUSE_MODE, toLockSeq, toLockResponse, "Mouse to Lock");
    // From Gesture:
    toLockResponse.setName("Gesture To Lock");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_MODE, toLockSeq, toLockResponse, "Gesture to Lock");
    // From Keyboard:
    toLockResponse.setName("Keyboard To Lock"); // OVERRIDE for Keyboard.
    toLockSeq[0] = (SeqElement(Pose::Type::doubleTap));
    toLockSeq[1] = (SeqElement(Pose::Type::waveIn));
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::KEYBOARD_MODE, toLockSeq, toLockResponse, "Keyboard to Lock");

    // Don't include "Hold Modes" in default Midas config

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }
}

int GestureFilter::defaultReturnFromHolds(void)
{
    sequence fromHoldGestSeq;
    fromHoldGestSeq.push_back(SeqElement(Pose::Type::rest, PoseLength::IMMEDIATE));
    CommandData fromHoldGestResponse;
    fromHoldGestResponse.setName("Release Hold");
    fromHoldGestResponse.setType(commandType::STATE_CHANGE);
    fromHoldGestResponse.setActionMode(midasMode::GESTURE_MODE);

    int ss = (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_ONE, fromHoldGestSeq, fromHoldGestResponse, "Release Hold");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_TWO, fromHoldGestSeq, fromHoldGestResponse, "Release Hold");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_THREE, fromHoldGestSeq, fromHoldGestResponse, "Release Hold");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FOUR, fromHoldGestSeq, fromHoldGestResponse, "Release Hold");
    ss |= (int)gestSeqRecorder->registerSequence(midasMode::GESTURE_HOLD_FIVE, fromHoldGestSeq, fromHoldGestResponse, "Release Hold");

    return ss;
}

void GestureFilter::handleStateChange(CommandData response, GestureFilter *gf)
{
    buzzFeedbackMode bfm = settingsSignaller.getBuzzFeedbackMode();
    if (bfm >= buzzFeedbackMode::MINIMAL)
    {
        if (gf->controlStateHandle->getMode() == LOCK_MODE || response.getAction().mode == LOCK_MODE)
        {
            gf->myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationMedium);
        }
        else if (bfm >= buzzFeedbackMode::ALLSTATECHANGES)
        {
            gf->myoStateHandle->peakMyo()->vibrateMyos(myo::Myo::VibrationType::vibrationShort);
        }
    }

    if (response.getType() != commandType::STATE_CHANGE)
    {
        // Should NOT have made it here
        return;
    }

    if (response.getAction().mode == midasMode::KEYBOARD_MODE || controlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        gestureSignaller.emitToggleKeyboard();
    }

    gestureSignaller.emitStateString(QTranslator::tr((modeToString(response.getAction().mode)).c_str()));

    std::cout << "Transitioning to state: " << response.getAction().mode << std::endl;
    controlStateHandle->setMode(response.getAction().mode);

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
		if (changeStateCommands[i].getType() == commandType::MOUSE_CMD)
		{
			dataMap = gf->handleMouseCommand(changeStateCommands[i]);
		}
		else if (changeStateCommands[i].getType() == commandType::KYBRD_CMD || changeStateCommands[i].getType() == commandType::KYBRD_GUI_CMD)
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

    if (response.getAction().profile == MOVE_PROFILE_FORWARD)
    {
        gestureSignaller.emitProfileChange(true);
    }
    else if (response.getAction().profile == MOVE_PROFILE_BACKWARD)
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
        if (response.getType() == commandType::STATE_CHANGE)
        {
            GestureFilter::handleStateChange(response, gf);
        }
        else if (response.getType() == commandType::MOUSE_CMD)
        {
            gf->handleMouseCommand(response);
        }
        else if (response.getType() == commandType::KYBRD_CMD)
        {
            gf->handleKybrdCommand(response, true);
        }
        else if (response.getType() == commandType::KYBRD_GUI_CMD)
        {
            gf->handleKybrdCommand(response, true);
        }
		else if (response.getType() == commandType::PROFILE_CHANGE)
		{
			//gf->handleProfileChangeCommand(response);
			GestureFilter::handleProfileChangeCommand(response, gf);

		}
    } while (true);
}

filterError GestureFilter::updateBasedOnProfile(ProfileManager& pm, std::string name)
{
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

    gestSeqRecorder->unregisterAll();
    // replace default return sequences from hold modes
    defaultReturnFromHolds();

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
            CommandData translatedCommand;// JORDEN TODO = getCDFromCommand(*cmdIt);
			translatedCommand.setName(it->name);
            translatedCommand.setType(profileCommandToCommandTypeMap[cmdIt->type]);
			// Currently only supporting one action, rather than a list.
			// The XML format supports a list so that it can be extended in Midas easily.
			std::string action = cmdIt->actions[0];
			switch (translatedCommand.getType())
			{
			case commandType::KYBRD_CMD:
				if (action.find("inputVector") == 0)
				{
					// special case where user could specify 0 or more keys to be pressed
					// in the format: "inputVector,ABCD..." where A, B, C, D... are all keys
					// intended to be added to the keyboardVector response.
					translatedCommand.setActionKybd(profileActionToKybd["inputVector"]);
					translatedCommand.setKeyboardVector(KeyboardVector::createFromProfileStr(action));
				}
				else
				{
                    translatedCommand.setActionKybd(profileActionToKybd[action]);
				}
				break;
			case commandType::KYBRD_GUI_CMD:
				if (profileActionToKybdGui.find(action) != profileActionToKybdGui.end())
				{
                    translatedCommand.setActionKybdGUI(profileActionToKybdGui[action]);
				}
				else
				{
                    translatedCommand.setType(commandType::KYBRD_CMD);
                    translatedCommand.setActionKybd(profileActionToKybd[action]);
				}
				break;
			case commandType::MOUSE_CMD:
                translatedCommand.setActionMouse(profileActionToMouseCommands[action]);
				break;
			case commandType::STATE_CHANGE:
				if (cmdItCount > 0)
				{
					throw new std::exception("GestureFilter: StateChangeAction set as StateChange");
				}
                translatedCommand.setActionMode(profileActionToStateChange[action]);
				break;
			case commandType::PROFILE_CHANGE:
                translatedCommand.setActionProfile(profileActionToProfileChange[action]);
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

    if (ss != (int)SequenceStatus::SUCCESS)
    {
        throw new std::exception("registerSequenceException");
    }

    gestSeqRecorder->updateGuiSequences();
    return filterError::NO_FILTER_ERROR;
}