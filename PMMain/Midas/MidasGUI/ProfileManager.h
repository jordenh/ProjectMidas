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

#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "MyoCommon.h"
#include "MidasCommon.h"
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct command {
    std::string type;
    std::vector<std::string> actions;
};

struct gesture {
    std::string type;
    std::string name;
};

struct profileSequence {
    std::string state;
    std::string name;
    std::vector<gesture> gestures;
    std::vector<command> cmds;
};

struct angleAction {
    std::string type;
    command anglePositive;
    command angleNegative;
    unsigned int sensitivity;
};

struct hold {
    std::string gesture;
    std::vector<angleAction> angles;
    std::string holdModeActionType;
    unsigned int intervalLen;
};

struct profile {
    std::string profileName;
    std::vector<profileSequence> profileSequences;
    std::vector<hold> holds;
};

extern std::map<std::string, commandType> profileCommandToCommandTypeMap;
extern std::map<std::string, kybdGUICmds> profileActionToKybdGui;
extern std::map<std::string, kybdCmds> profileActionToKybd;
extern std::map<std::string, mouseCmds> profileActionToMouseCommands;
extern std::map<std::string, midasMode> profileActionToStateChange;
extern std::map<std::string, profileCmds> profileActionToProfileChange;
extern std::map <std::string, PoseLength> profileGestureTypeToPoseLength;
extern std::map <std::string, Pose::Type> profileGestureNameToType;
extern std::map <std::string, HoldModeActionType> holdModeActionTypeMap;

class ProfileManager {

public:
    ProfileManager();
    ~ProfileManager();

    void loadProfilesFromFile(std::string fileName);

    std::vector<profile>* getProfiles();

private:
    profile extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName);

    profileSequence extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
        std::string sequenceState, std::string sequenceName);

    std::vector<profile> profiles;
};

#endif /* PROFILE_MANAGER_H */