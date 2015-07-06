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

#include "CommandData.h"


CommandData::~CommandData()
{
}

int CommandData::addChangeStateAction(CommandData cmd)
{
	if(( this->type != commandType::STATE_CHANGE) || (cmd.type == commandType::STATE_CHANGE))
	{
		// error - can't add state_change or else infinite loops possible.
		// or error - can only add action to state_change commands.
		return 1;
	}

	changeStateActions.push_back(cmd);

	return 0;
}

void CommandData::clearChangeStateActions()
{
	changeStateActions.clear();
}