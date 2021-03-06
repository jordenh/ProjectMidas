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

#include "ProfileManager.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <fstream>

std::map<std::string, commandType> profileCommandToCommandTypeMap =
{
    { "keyboard", KYBRD_CMD },
    { "keyboardGui", KYBRD_GUI_CMD },
    { "mouse", MOUSE_CMD },
    { "stateChange", STATE_CHANGE },
	{ "profileChange", PROFILE_CHANGE },
    { "none", NONE }
};

std::map<std::string, kybdGUICmds> profileActionToKybdGui =
{
    { "swapRingFocus", SWAP_RING_FOCUS },
    { "changeWheels", CHANGE_WHEELS },
    { "select", SELECT },
    { "holdSelect", HOLD_SELECT },
};

std::map<std::string, kybdCmds> profileActionToKybd =
{
    { "none", NO_CMD },
    { "undo", UNDO },
    { "redo", REDO },
    { "zoomIn", ZOOM_IN },
    { "zoomOut", ZOOM_OUT },
    { "zoom100", ZOOM_100 },
    { "escape", ESCAPE },
    { "enter", ENTER },
    { "tab", TAB },
    { "switchWinForward", SWITCH_WIN_FORWARD },
    { "switchWinReverse", SWITCH_WIN_REVERSE },
    { "copy", COPY },
    { "paste", PASTE },
    { "cut", CUT },
    { "fileMenu", FILE_MENU },
    { "newBrowser", NEW_BROWSER },
    { "gotoAddrBar", GOTO_ADDR_BAR },
    { "lockDesktop", LOCK_DESKTOP },
    { "editMenu", EDIT_MENU },
    { "viewMenu", VIEW_MENU },
    { "winHome", WIN_HOME },
    { "hideApps", HIDE_APPS },
    { "control", CONTROL },
    { "volumeUp", VOLUME_UP },
    { "volumeDown", VOLUME_DOWN },
    { "backspace", BACKSPACE },
    { "upArrow", UP_ARROW },
    { "downArrow", DOWN_ARROW },
    { "rightArrow", RIGHT_ARROW },
    { "leftArrow", LEFT_ARROW },
	{ "inputVector", INPUT_VECTOR }
};

std::map<std::string, mouseCmds> profileActionToMouseCommands =
{
    { "leftClick", LEFT_CLICK },
    { "rightClick", RIGHT_CLICK },
    { "middleClick", MIDDLE_CLICK },
    { "leftHold", LEFT_HOLD },
    { "rightHold", RIGHT_HOLD },
    { "middleHold", MIDDLE_HOLD },
    { "moveLeft", MOVE_LEFT },
    { "moveRight", MOVE_RIGHT },
    { "moveUp", MOVE_UP },
    { "moveDown", MOVE_DOWN },
    { "moveHor", MOVE_HOR },
    { "moveVert", MOVE_VERT },
    { "scrollLeft", SCROLL_LEFT },
    { "scrollRight", SCROLL_RIGHT },
    { "scrollUp", SCROLL_UP },
    { "scrollDown", SCROLL_DOWN },
    { "shiftScrollUp", SHIFT_SCROLL_UP },
    { "shiftScrollDown", SHIFT_SCROLL_DOWN },
    { "leftRelease", LEFT_RELEASE },
    { "rightRelease", RIGHT_RELEASE },
    { "middleRelease", MIDDLE_RELEASE },
    { "releaseLrmButs", RELEASE_LRM_BUTS }
};

std::map<std::string, midasMode> profileActionToStateChange =
{
    { "lockMode", LOCK_MODE },
    { "mouseMode", MOUSE_MODE },
    { "mouseMode2", MOUSE_MODE2 },
    { "keyboardMode", KEYBOARD_MODE },
    { "gestureMode", GESTURE_MODE },
    { "gestureHoldOne", GESTURE_HOLD_ONE }, // DT
    { "gestureHoldTwo", GESTURE_HOLD_TWO }, // spread
    { "gestureHoldThree", GESTURE_HOLD_THREE }, // fist
    { "gestureHoldFour", GESTURE_HOLD_FOUR }, //wavein
    { "gestureHoldFive", GESTURE_HOLD_FIVE } //waveout
};

std::map<std::string, profileCmds> profileActionToProfileChange =
{
	{ "moveProfileForward", MOVE_PROFILE_FORWARD },
	{ "moveProfileBackward", MOVE_PROFILE_BACKWARD }
};

std::map <std::string, PoseLength> profileGestureTypeToPoseLength =
{
    { "tap", PoseLength::TAP },
    { "hold", PoseLength::HOLD },
    { "immediate", PoseLength::IMMEDIATE }
};

std::map <std::string, Pose::Type> profileGestureNameToType =
{
    { "fingersSpread", Pose::Type::fingersSpread },
    { "fist", Pose::Type::fist },
    { "rest", Pose::Type::rest },
    { "doubleTap", Pose::Type::doubleTap },
    { "waveIn", Pose::Type::waveIn },
    { "waveOut", Pose::Type::waveOut }
};

std::map <std::string, HoldModeActionType> holdModeActionTypeMap =
{
    { "absDeltaFinite", ABS_DELTA_FINITE },
    { "absDeltaVelocity", ABS_DELTA_VELOCITY },
    { "intervalDelta", INTERVAL_DELTA }
};

ProfileManager::ProfileManager() { }

ProfileManager::~ProfileManager() { }

void ProfileManager::loadProfilesFromFile(std::string fileName)
{
    using boost::property_tree::ptree;

    profiles.clear();

    std::ifstream profileFile(fileName.c_str());

    ptree tr;
    try {
        read_xml(profileFile, tr);
    }
    catch (const std::exception& e) {
        // TODO - log error? 
        return;
    }

    BOOST_FOREACH(const ptree::value_type & vt, tr.get_child("profiles")) {
        if (vt.first == "profile")
        {
            std::string profileName = vt.second.get<std::string>("<xmlattr>.name");
            profiles.push_back(extractProfileInformation(vt, profileName));
        }
    }

    profileFile.close();
}

std::vector<profile>* ProfileManager::getProfiles()
{
    return &profiles;
}

profile ProfileManager::extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName)
{
    using boost::property_tree::ptree;

    profile pr; 
    pr.profileName = profileName;

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("sequences")) 
    {
        if (vt.first == "sequence")
        {
            std::string sequenceStateBegin = vt.second.get<std::string>("<xmlattr>.state");
            std::string sequenceName = vt.second.get<std::string>("<xmlattr>.name");
            pr.profileSequences.push_back(extractSequenceInformation(vt, sequenceStateBegin, sequenceName));
        }
    }

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("holds"))
    {
        if (vt.first == "hold")
        {
            hold currHold;
            unsigned int holdID = vt.second.get<int>("<xmlattr>.id");
            currHold.id = holdID;

            BOOST_FOREACH(const ptree::value_type & angleVt, vt.second)
            {
                if (angleVt.first == "angle")
                {
                    angleAction currAngleAction;
                    std::string angleType = angleVt.second.get<std::string>("<xmlattr>.type");

                    BOOST_FOREACH(const ptree::value_type & posAngleTree, angleVt.second.get_child("anglePositive")) {
                        if (posAngleTree.first == "command")
                        {
                            currAngleAction.anglePositive.type = posAngleTree.second.get<std::string>("<xmlattr>.type");
                        }

                        BOOST_FOREACH(const ptree::value_type & vt, posAngleTree.second.get_child("actions")) {
                            if (vt.first == "action")
                            {
                                currAngleAction.anglePositive.actions.push_back(vt.second.get_value<std::string>());
                            }
                        }
                    }

                    BOOST_FOREACH(const ptree::value_type & posAngleTree, angleVt.second.get_child("angleNegative")) {
                        if (posAngleTree.first == "command")
                        {
                            currAngleAction.angleNegative.type = posAngleTree.second.get<std::string>("<xmlattr>.type");
                        }

                        BOOST_FOREACH(const ptree::value_type & vt, posAngleTree.second.get_child("actions")) {
                            if (vt.first == "action")
                            {
                                currAngleAction.angleNegative.actions.push_back(vt.second.get_value<std::string>());
                            }
                        }
                    }

                    float angleSensitivity = angleVt.second.get_child("sensitivity").get_value<float>();
                    currAngleAction.sensitivity = angleSensitivity;
                    currAngleAction.type = angleType;

                    currHold.angles.push_back(currAngleAction);

                    // Legacy method! (Files earlier than July 29, 2015)
                //    angleAction currAngleAction;
                //    std::string angleType = angleVt.second.get<std::string>("<xmlattr>.type");
                //    std::string anglePositive = angleVt.second.get_child("anglePositive").get_value<std::string>();
                //    std::string angleNegative = angleVt.second.get_child("angleNegative").get_value<std::string>();
                //    unsigned int angleSensitivity = angleVt.second.get_child("sensitivity").get_value<unsigned int>();
                //    currAngle.anglePositive = anglePositive;
                //    currAngle.angleNegative = angleNegative;
                //    currAngle.sensitivity = angleSensitivity;
                //    currAngle.type = angleType;
                //    currHold.angles.push_back(currAngle);
                }
            }

            std::string holdModeActionType = vt.second.get_child("holdModeActionType").get_value<std::string>();
            unsigned int intervalLen = vt.second.get_child("intervalLength").get_value<unsigned int>();
            unsigned int intervalExecMult = vt.second.get_child("intervalExecMultiplier").get_value<unsigned int>();
            unsigned int intervalMaxExecs = vt.second.get_child("intervalMaxExecs").get_value<unsigned int>();
            currHold.holdModeActionType = holdModeActionType;
            currHold.intervalLen = intervalLen;
            currHold.intervalExecMultiplier = intervalExecMult;
            currHold.intervalMaxExecs = intervalMaxExecs;

            pr.holds.push_back(currHold);
        }
    }

    return pr;
}

profileSequence ProfileManager::extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
    std::string sequenceState, std::string sequenceName)
{
    using boost::property_tree::ptree;

    profileSequence seq;
    std::vector<gesture> gestures;

    BOOST_FOREACH(const ptree::value_type & vt, parentSequence.second.get_child("gestures")) {
        if (vt.first == "gesture")
        {
            gesture gest;
            gest.type = vt.second.get<std::string>("<xmlattr>.type");
            gest.name = vt.second.get_value<std::string>();
            
            gestures.push_back(gest);
        }
    }

	std::vector<command> cmds;
	BOOST_FOREACH(const ptree::value_type & pt, parentSequence.second.get_child("commands")) {
		command cmd;
		if (pt.first == "command")
		{
			cmd.type = pt.second.get<std::string>("<xmlattr>.type");
		}

		BOOST_FOREACH(const ptree::value_type & vt, pt.second.get_child("actions")) {
			if (vt.first == "action")
			{
				cmd.actions.push_back(vt.second.get_value<std::string>());
			}
		}
		cmds.push_back(cmd);
	}

    seq.cmds = cmds;
    seq.gestures = gestures;
    seq.state = sequenceState;
    seq.name = sequenceName;

    return seq;
}