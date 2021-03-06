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

#ifndef COMMAND_DATA_H
#define COMMAND_DATA_H

#include "MidasCommon.h"
#include "KeyboardVector.h"

/**
* This bundles the Midas command data. It represents
* all Midas command types.
*/
class CommandData
{
public:
    /* 
     * Basic constructors/destructor
     */
	CommandData()
	{
		this->type = commandType::NONE;
		this->name = "";
	}

	CommandData(commandType type, action action, std::string name, KeyboardVector vec)
	{
		this->type = type;
		this->actn = action;
		this->name = name;
		this->keyboardVector = vec;
	}
	~CommandData();

	/*
    * Used to append a command into the vector of changeStateActions.
    * These are used to execute commands immediately after a state change
    * which can be usedful (ex: click a mouse immediately when entering mouse mode)
    *
    * @param cmd The command to execute on statechange
	* @return 0 for success, positive otherwise
	*/
	int addChangeStateAction(CommandData cmd);

    /* 
     * Clears the vecotr of actions
     */
	void clearChangeStateActions();

    /* 
     * Accessor
     *
     * @return std::vector<CommandData> the vector of actions to perform when a state is changed
     */
	std::vector<CommandData> getChangeStateActions() { return changeStateActions; }

    // Accessor / Mutators
    void setType(commandType type);
    commandType getType();

    void setAction(action actn);
    action getAction();

    void setActionKybd(kybdCmds kybd);
    void setActionKybdGUI(kybdGUICmds kybdGUI);
    void setActionMouse(mouseCmds mouse);
    void setActionMode(midasMode mode);
    void setActionProfile(profileCmds profile);

    void setName(std::string name);
    std::string getName();

    void setKeyboardVector(KeyboardVector keyVec);
    KeyboardVector getKeyboardVector();

private:
    commandType type;
    action actn;
    
    std::string name;
    KeyboardVector keyboardVector;


	// these actions will be executed as soon as the state is changed,
	// in the order they are added.
	std::vector<CommandData> changeStateActions;
};

#endif COMMAND_DATA_H