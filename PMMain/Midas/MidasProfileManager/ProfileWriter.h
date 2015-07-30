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

#ifndef PROFILE_WRITER_H
#define PROFILE_WRITER_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct Command {
    std::string type;
    std::vector<std::string> actions;
};

struct Gesture {
    std::string type;
    std::string name;
};

struct Sequence {
    std::string state;
    std::string name;
    std::vector<Gesture> gestures;
	std::vector<Command> cmds;
};

struct AngleAction {
    std::string type;
    Command anglePositive;
    Command angleNegative;
    float sensitivity;
};

struct Hold {
    std::string gesture;
    std::vector<AngleAction> angles;
    std::string holdModeActionType;
    unsigned int intervalLen;
};

struct Profile {
    std::string profileName;
    std::vector<Sequence> sequences;
    std::vector<Hold> holds;
};

class ProfileWriter
{
public:
    ProfileWriter();
    ~ProfileWriter();

    void writeProfiles(std::string filename, std::vector<Profile> profiles);
    std::vector<Profile> loadProfilesFromFile(std::string fileName);

private:
    void writeProfile(boost::property_tree::ptree &profileNode, Profile profile);
    void writeSequence(boost::property_tree::ptree &sequenceNode, Sequence sequence);
    Profile extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName);
    Sequence extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence, 
        std::string sequenceState, std::string sequenceName);
};

#endif /* PROFILE_WRITER_H */