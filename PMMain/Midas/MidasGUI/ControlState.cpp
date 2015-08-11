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

#include "ControlState.h"

#include "SharedCommandData.h"

std::mutex ControlState::currentlyHeldMutex;

ControlState::ControlState(SharedCommandData* SCDHandle)
{
    this->SCDHandle = SCDHandle;
    currentMode = LOCK_MODE;
    mouseCurrentlyHeld = false;
    keyCurrentlyHeld = false;
	currentProfile = "default";
}


ControlState::~ControlState()
{
}


void ControlState::setMode(midasMode mode)
{
    // Note that empty() uses a command Queue Mutex to protect
    // the SharedCommandData integrity.
    currentMode = mode;
    SCDHandle->empty();
}

bool ControlState::trySetMode(midasMode mode)
{
    // Note that tryEmpty() uses a command Queue Mutex to protect
    // the SharedCommandData integrity.
    
	if (SCDHandle->tryEmpty()) {
		currentMode = mode;
	}
	else {
		return false;
	}
    return true;
}

midasMode ControlState::getMode()
{
    return currentMode;
}

void ControlState::setMouseCurrentlyHeld(bool held)
{
    currentlyHeldMutex.lock();
    mouseCurrentlyHeld = held;
    currentlyHeldMutex.unlock();
}

bool ControlState::trySetMouseCurrentlyHeld(bool held)
{
    if (currentlyHeldMutex.try_lock())
    {
        mouseCurrentlyHeld = held;
    }
    else 
    {
        return false;
    }

    currentlyHeldMutex.unlock();
    return true;
}

bool ControlState::getMouseCurrentlyHeld()
{
    currentlyHeldMutex.lock();
    bool retVal = mouseCurrentlyHeld;
    currentlyHeldMutex.unlock();
    return retVal;
}

void ControlState::setKeyCurrentlyHeld(bool held)
{
    currentlyHeldMutex.lock();
    keyCurrentlyHeld = held;
    currentlyHeldMutex.unlock();
}

bool ControlState::trySetKeyCurrentlyHeld(bool held)
{
    if (currentlyHeldMutex.try_lock())
    {
        keyCurrentlyHeld = held;
    }
    else
    {
        return false;
    }

    currentlyHeldMutex.unlock();
    return true;
}

bool ControlState::getKeyCurrentlyHeld()
{
    currentlyHeldMutex.lock();
    bool retVal = keyCurrentlyHeld;
    currentlyHeldMutex.unlock();
    return retVal;
}

void ControlState::setProfile(std::string profile)
{
	currentProfile = profile;
}

std::string ControlState::getProfile()
{
	return currentProfile;
}