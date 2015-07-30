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

#ifndef PROFILE_COMMON_H
#define PROFILE_COMMON_H

#include "CommandData.h"
#include "ProfileManager.h"

CommandData getCDFromCommand(command cmd)
{
    CommandData translatedCommand;
    translatedCommand.setName("defaultName");
    translatedCommand.setType(profileCommandToCommandTypeMap[cmd.type]);
    // Currently only supporting one action, rather than a list.
    // The XML format supports a list so that it can be extended in Midas easily.
    std::string action = cmd.actions[0];
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
        translatedCommand.setActionMode(profileActionToStateChange[action]);
        break;
    case commandType::PROFILE_CHANGE:
        translatedCommand.setActionProfile(profileActionToProfileChange[action]);
    default:
        break;
    }

    return translatedCommand;
}

#endif