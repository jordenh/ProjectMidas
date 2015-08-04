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

#include "ProfileWriter.h"
#include <fstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

ProfileWriter::ProfileWriter()
{
}


ProfileWriter::~ProfileWriter()
{
}

void ProfileWriter::writeProfiles(std::string filename, std::vector<Profile> profiles)
{
    using boost::property_tree::ptree;

    ptree tr;

    BOOST_FOREACH(Profile profile, profiles)
    {
        ptree &profileNode = tr.add("profiles.profile", "");
		if (profile.profileName.size() > 0)
		{
			profileNode.put("<xmlattr>.name", profile.profileName);
		}
		else
		{
			profileNode.put("<xmlattr>.name", "default");
		}
        writeProfile(profileNode, profile);
    }

    write_xml(filename, tr, std::locale(), boost::property_tree::xml_writer_make_settings<ptree::key_type>(' ', 4u));
}

void ProfileWriter::writeProfile(boost::property_tree::ptree &profileNode, Profile profile)
{
    using boost::property_tree::ptree;

    if (profile.sequences.empty())
    {
        profileNode.add("sequences", "");
    }
    else
    {
        BOOST_FOREACH(Sequence sequence, profile.sequences)
        {
            ptree &sequenceNode = profileNode.add("sequences.sequence", "");
            sequenceNode.put("<xmlattr>.state", sequence.state);
            sequenceNode.put("<xmlattr>.name", sequence.name);
            writeSequence(sequenceNode, sequence);
        }
    }

    if (profile.holds.empty())
    {
        profileNode.add("holds", "");
    }
    else
    {
        BOOST_FOREACH(Hold hold, profile.holds)
        {
            ptree &holdNode = profileNode.add("holds.hold", "");
            holdNode.put("<xmlattr>.id", hold.id);
            
            BOOST_FOREACH(AngleAction angleAction, hold.angles)
            {
                ptree &angleNode = holdNode.add("angle", "");
                angleNode.put("<xmlattr>.type", angleAction.type);

                ptree &anglePosNode = angleNode.add("anglePositive", "");
                ptree &commandNode = anglePosNode.add("command", "");
                commandNode.put("<xmlattr>.type", angleAction.anglePositive.type);
                BOOST_FOREACH(std::string action, angleAction.anglePositive.actions)
                {
                    commandNode.add("actions.action", action);
                }
                ptree &angleNegNode = angleNode.add("angleNegative", "");
                ptree &commandNode2 = angleNegNode.add("command", "");
                commandNode2.put("<xmlattr>.type", angleAction.angleNegative.type);
                BOOST_FOREACH(std::string action, angleAction.angleNegative.actions)
                {
                    commandNode2.add("actions.action", action);
                }

                angleNode.add("sensitivity", angleAction.sensitivity);
            }
            holdNode.add("holdModeActionType", hold.holdModeActionType);
            holdNode.add("intervalLength", hold.intervalLen);
            holdNode.add("intervalExecMultiplier", hold.intervalExecMultiplier);
            holdNode.add("intervalMaxExecs", hold.intervalMaxExecs);
        }
    }
}

void ProfileWriter::writeSequence(boost::property_tree::ptree &sequenceNode, Sequence sequence)
{
    using boost::property_tree::ptree;

    BOOST_FOREACH(Gesture gesture, sequence.gestures)
    {
        ptree &gestureNode = sequenceNode.add("gestures.gesture", gesture.name);
        gestureNode.put("<xmlattr>.type", gesture.type);
    }

	BOOST_FOREACH(Command command, sequence.cmds)
	{
		ptree &commandNode = sequenceNode.add("commands.command", "");
		commandNode.put("<xmlattr>.type", command.type);

		BOOST_FOREACH(std::string action, command.actions)
		{
			commandNode.add("actions.action", action);
		}
	}

    //ptree &cmdNode = sequenceNode.add("command", "");
    //cmdNode.put("<xmlattr>.type", sequence.cmd.type);
	//
    //BOOST_FOREACH(std::string action, sequence.cmd.actions)
    //{
    //    cmdNode.add("actions.action", action);
    //}
}

std::vector<Profile> ProfileWriter::loadProfilesFromFile(std::string fileName)
{
    using boost::property_tree::ptree;

    std::vector<Profile> profiles;

    std::ifstream profileFile(fileName.c_str());

    ptree tr;
    try
    {
        read_xml(profileFile, tr);

        BOOST_FOREACH(const ptree::value_type & vt, tr.get_child("profiles")) {
            if (vt.first == "profile")
            {
                std::string profileName = vt.second.get<std::string>("<xmlattr>.name");
                profiles.push_back(extractProfileInformation(vt, profileName));
            }
        }
    }
    catch (...)
    {
        std::vector<Profile> empty;
        return empty;
    }

    profileFile.close();

    return profiles;
}

Profile ProfileWriter::extractProfileInformation(const boost::property_tree::ptree::value_type & parentProfile, std::string profileName)
{
    using boost::property_tree::ptree;

    Profile pr;
    pr.profileName = profileName;

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("sequences")) {
        if (vt.first == "sequence")
        {
            std::string sequenceStateBegin = vt.second.get<std::string>("<xmlattr>.state");
            std::string sequenceName = vt.second.get<std::string>("<xmlattr>.name");
            pr.sequences.push_back(extractSequenceInformation(vt, sequenceStateBegin, sequenceName));
        }
    }

    BOOST_FOREACH(const ptree::value_type & vt, parentProfile.second.get_child("holds"))
    {
        if (vt.first == "hold")
        {
            Hold currHold;
            unsigned int holdID = vt.second.get<int>("<xmlattr>.id");
            currHold.id = holdID;

            BOOST_FOREACH(const ptree::value_type & angleVt, vt.second)
            {
                if (angleVt.first == "angle")
                {
                    AngleAction currAngleAction;
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
                }
            }

            std::string holdModeActionType = vt.second.get_child("holdModeActionType").get_value<std::string>();
            unsigned int intervalLen = vt.second.get_child("intervalLength").get_value<unsigned int>();
            unsigned int intervalExecMultiplier = vt.second.get_child("intervalExecMultiplier").get_value<unsigned int>();
            unsigned int intervalMaxExecs = vt.second.get_child("intervalMaxExecs").get_value<unsigned int>();
            currHold.holdModeActionType = holdModeActionType;
            currHold.intervalLen = intervalLen;
            currHold.intervalExecMultiplier = intervalExecMultiplier;
            currHold.intervalMaxExecs = intervalMaxExecs;
            pr.holds.push_back(currHold);
        }
    }

    return pr;
}

Sequence ProfileWriter::extractSequenceInformation(const boost::property_tree::ptree::value_type & parentSequence,
    std::string sequenceState, std::string sequenceName)
{
    using boost::property_tree::ptree;

    Sequence seq;
    std::vector<Gesture> gestures;

    BOOST_FOREACH(const ptree::value_type & vt, parentSequence.second.get_child("gestures")) {
        if (vt.first == "gesture")
        {
            Gesture gest;
            gest.type = vt.second.get<std::string>("<xmlattr>.type");
            gest.name = vt.second.get_value<std::string>();

            gestures.push_back(gest);
        }
    }

	std::vector<Command> cmds;
	BOOST_FOREACH(const ptree::value_type & vt, parentSequence.second.get_child("commands")) {
		if (vt.first == "command")
		{
			Command cmd;
			cmd.type = vt.second.get<std::string>("<xmlattr>.type");

			BOOST_FOREACH(const ptree::value_type & action_vt, vt.second.get_child("actions")) {
				if (action_vt.first == "action")
				{
					cmd.actions.push_back(action_vt.second.get_value<std::string>());
				}
			}
			cmds.push_back(cmd);
		}
	}

    seq.cmds = cmds;
    seq.gestures = gestures;
    seq.state = sequenceState;
    seq.name = sequenceName;

    return seq;
}