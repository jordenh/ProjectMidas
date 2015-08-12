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

#ifndef _CONTROL_STATE_H
#define _CONTROL_STATE_H

#include <mutex>
#include "MidasCommon.h"

class SharedCommandData;

/**
 * This class keeps track of the current state (or mode) of the Midas application. This
 * state is used to determine the actions the user can or cannot perform. For example, in mouse mode,
 * the user may move around the cursor with the device. In locked mode, the cursor will
 * not move.
 */
class ControlState
{
public:

    /**
     * The constructor for ControlState.
     * The contructor must have a handle on the SharedCommandData so that 
     * unprocessed commands can be removed from the command queue when the 
     * state of the application changes.
     *
     * @param SCDHandle A handle to the SharedCommandData object that is used by the
     * main thread and device threads.
     */
    ControlState(SharedCommandData* SCDHandle);
    ~ControlState();

    /**
     * Changes the mode of the Midas application. This is a thread-safe operation, but
     * it blocks on the command queue mutex. If the command queue is being modified by another 
     * thread, this will not return until the other thread is finished.
     *
     * @param mode The new mode.
     */
    void setMode(midasMode mode);

    /**
     * Changes the mode of the Midas application. If the command queue is being modified
     * by another thread, this will exit right away with a return value of false. Otherwise,
     * it will change the mode and return true.
     *
     * @param mode The new mode.
     * @return True if the mode was successfully changed; false otherwise.
     */
    bool trySetMode(midasMode mode);

    /**
     * Gets the current mode of the Midas application.
     *
     * @return The current mode.
     */
    midasMode getMode();

    void setMouseCurrentlyHeld(bool held);
    bool trySetMouseCurrentlyHeld(bool held);
    bool getMouseCurrentlyHeld();

	void setProfile(std::string profile);
	std::string getProfile();

	SharedCommandData* getSCD() { return SCDHandle; }

private:
    midasMode currentMode;
    static std::mutex currentlyHeldMutex;
    bool mouseCurrentlyHeld;
	std::string currentProfile;
    SharedCommandData *SCDHandle; // not owned
};

#endif /* _CONTROL_STATE_H */