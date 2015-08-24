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

#include "SharedCommandData.h"

void SharedCommandData::addCommand(CommandData dat)
{
    commandQueueMutex.lock();
    commandQueue.push(dat);
    commandQueueMutex.unlock();
}

bool SharedCommandData::tryAddCommand(CommandData dat)
{
    bool locked = commandQueueMutex.try_lock();
    if (locked) 
    {
        commandQueue.push(dat);
        commandQueueMutex.unlock();
    }

    return locked;
}

bool SharedCommandData::consumeCommand(CommandData& dat)
{
    if (commandQueue.empty()) return false;
    commandQueueMutex.lock();
    dat = commandQueue.front();
    commandQueue.pop();
    commandQueueMutex.unlock();

    return true;
}

bool SharedCommandData::tryConsumeCommand(CommandData& outCommandData)
{
    bool locked = commandQueueMutex.try_lock();
    if (locked)
    {
        outCommandData = commandQueue.front();
        commandQueue.pop();
        commandQueueMutex.unlock();
    }

    return locked;
}

void SharedCommandData::setVelocity(point velocity)
{
    velocityMutex.lock();
    mouseVelocity = velocity;
    velocityMutex.unlock();
}

bool SharedCommandData::trySetVelocity(point velocity)
{
    bool locked = velocityMutex.try_lock();
    if (locked) {
        mouseVelocity = velocity;
        velocityMutex.unlock();
    }

    return locked;
}

point SharedCommandData::getVelocity()
{
    velocityMutex.lock();
    point velocity = mouseVelocity;
    velocityMutex.unlock();

    return velocity;
}

bool SharedCommandData::tryGetVelocity(point& outVelocity)
{
    bool locked = velocityMutex.try_lock();
    if (locked) {
        outVelocity = mouseVelocity;
        velocityMutex.unlock();
    }

    return locked;
}

void SharedCommandData::setKybdGuiSel(unsigned int kybdGuiSel)
{
	kybdGuiSelMutex.lock();
	if (kybdGuiSel <= maxKybdGuiSel)
	{
		this->kybdGuiSel = kybdGuiSel;
	}
	kybdGuiSelMutex.unlock();
}

bool SharedCommandData::trySetKybdGuiSel(unsigned int kybdGuiSel)
{
	bool locked = kybdGuiSelMutex.try_lock();
	if (locked) {
		if (kybdGuiSel <= maxKybdGuiSel)
		{
			this->kybdGuiSel = kybdGuiSel;
			kybdGuiSelMutex.unlock();
		}
	}
	return locked;
}

unsigned int SharedCommandData::getKybdGuiSel()
{
	kybdGuiSelMutex.lock();
	unsigned int guiSel = kybdGuiSel;
	kybdGuiSelMutex.unlock();

	return guiSel;
}

bool SharedCommandData::tryGetKybdGuiSel(unsigned int& outKybdGuiSel)
{
	bool locked = kybdGuiSelMutex.try_lock();
	if (locked) {
		outKybdGuiSel = kybdGuiSel;
		kybdGuiSelMutex.unlock();
	}

	return locked;
}

unsigned int SharedCommandData::getKybdGuiSelMax()
{
	kybdGuiSelMutex.lock();
	unsigned int max = maxKybdGuiSel;
	kybdGuiSelMutex.unlock();

	return max;
}

bool SharedCommandData::tryGetKybdGuiSelMax(unsigned int& outMaxKybdGuiSel)
{
	bool locked = kybdGuiSelMutex.try_lock();
	if (locked) {
		outMaxKybdGuiSel = maxKybdGuiSel;
		kybdGuiSelMutex.unlock();
	}

	return locked;
}

void SharedCommandData::setKeySelectAngle(keyboardAngle angle)
{
	keySelectAngleMutex.lock();
	keySelectAngle = angle;
	keySelectAngleMutex.unlock();
}

bool SharedCommandData::trySetKeySelectAngle(keyboardAngle angle)
{
	bool locked = keySelectAngleMutex.try_lock();
	if (locked) {
		keySelectAngle = angle;
		keySelectAngleMutex.unlock();
	}

	return locked;
}

keyboardAngle SharedCommandData::getKeySelectAngle()
{
	keySelectAngleMutex.lock();
	keyboardAngle angle = keySelectAngle;
	keySelectAngleMutex.unlock();

	return angle;
}


bool SharedCommandData::tryGetKeySelectAngle(keyboardAngle& outKeySelectAngle)
{
	bool locked = keySelectAngleMutex.try_lock();
	if (locked) {
		outKeySelectAngle = keySelectAngle;
		keySelectAngleMutex.unlock();
	}

	return locked;
}

int SharedCommandData::getRssi()
{
	rssiMutex.lock();
	int rssi = rssiAVG;
	rssiMutex.unlock();
	return rssi;
}

void SharedCommandData::setRssi(int rssi)
{
	rssiMutex.lock();
	rssiAVG = rssi;
	rssiMutex.unlock();
}

unsigned int SharedCommandData::getBatteryLevel()
{
    batteryLevelMutex.lock();
    unsigned int retVal = this->batteryLevel;
    batteryLevelMutex.unlock();
    return retVal;
}

void SharedCommandData::setBatteryLevel(unsigned int batteryLevel)
{
    batteryLevelMutex.lock();
    this->batteryLevel = batteryLevel;
    batteryLevelMutex.unlock();
}

bool SharedCommandData::getIsConnected()
{
    isConnectedMutex.lock();
    bool connected = isConnected;
    isConnectedMutex.unlock();
    return connected;
}

void SharedCommandData::setIsConnected(bool connected)
{
    isConnectedMutex.lock();
    isConnected = connected;
    isConnectedMutex.unlock();
}

bool SharedCommandData::getIsSynched(void)
{
    isSynchedMutex.lock();
    bool synched = isSynched;
    isSynchedMutex.unlock();
    return synched;
}

void SharedCommandData::setIsSynched(bool synched)
{
    isSynchedMutex.lock();
    isSynched = synched;
    isSynchedMutex.unlock();
}

bool SharedCommandData::isCommandQueueEmpty()
{
    return commandQueue.empty();
}

void SharedCommandData::process()
{
    filterDataMap input = Filter::getInput();
    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::setFilterStatus(filterStatus::END_CHAIN);

    if (input.find(COMMAND_INPUT) != input.end())
    {
        boost::any value = input[COMMAND_INPUT];
        extractCommand(value);
    }
    
    if (input.find(VELOCITY_INPUT) != input.end())
    {
        boost::any value = input[VELOCITY_INPUT];
        extractPoint(value);
    }

	if (input.find(DELTA_INPUT) != input.end())
	{
		boost::any value = input[DELTA_INPUT];
		extractVector2D(value);
	}

    if (input.find(ISCONNECTED_INPUT) != input.end())
    {
        boost::any value = input[ISCONNECTED_INPUT];
        extractIsConnected(value);
    }

    if (input.find(IMPULSE_STATUS) != input.end())
    {
        boost::any value = input[IMPULSE_STATUS];
        extractImpulseStatus(value);
    }

    if (input.find(SYNCHED_INPUT) != input.end())
    {
        boost::any value = input[SYNCHED_INPUT];
        extractIsSynched(value);
    }

    if (input.find(ANGLE_INPUT) != input.end())
    {
	    boost::any value = input[ANGLE_INPUT];
	    extractKeySelectAngle(value);
    }
	
    if (input.find(RSSI_INPUT) != input.end())
    {
	    boost::any value = input[RSSI_INPUT];
	    extractRssi(value);
    }

    if (input.find(BATTERY_LEVEL_INPUT) != input.end())
    {
        boost::any value = input[BATTERY_LEVEL_INPUT];
        extractBattery(value);
    }
}

void SharedCommandData::empty()
{
    commandQueueMutex.lock();
    commandQueue.empty();
    commandQueueMutex.unlock();
}

bool SharedCommandData::tryEmpty()
{
    bool locked = commandQueueMutex.try_lock();

    if (locked)
    {
        commandQueue.empty();
        commandQueueMutex.unlock();
    }

    return locked;
}

void SharedCommandData::extractCommand(boost::any value)
{
    if (value.type() != typeid(CommandData)) 
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        CommandData data = boost::any_cast<CommandData>(value);
        addCommand(data);
    }
}

void SharedCommandData::extractPoint(boost::any value)
{
    if (value.type() != typeid(point))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        point velocity = boost::any_cast<point>(value);
        setVelocity(velocity);
    }
}

void SharedCommandData::extractKeySelectAngle(boost::any value)
{
	if (value.type() != typeid(keyboardAngle))
	{
		Filter::setFilterError(filterError::INVALID_INPUT);
		Filter::setFilterStatus(filterStatus::FILTER_ERROR);
	}
	else
	{
		keyboardAngle angle = boost::any_cast<keyboardAngle> (value);
		setKeySelectAngle(angle);
	}
}

void SharedCommandData::extractRssi(boost::any value)
{
	if (value.type() != typeid(int))
	{
		Filter::setFilterError(filterError::INVALID_INPUT);
		Filter::setFilterStatus(filterStatus::FILTER_ERROR);
	}
	else
	{
		int rssi = boost::any_cast<int> (value);
		setRssi(rssi);
	}
}

void SharedCommandData::extractBattery(boost::any value)
{
    if (value.type() != typeid(unsigned int))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        unsigned int batteryLevel = boost::any_cast<unsigned int> (value);
        setBatteryLevel(batteryLevel);
    }
}

void SharedCommandData::extractIsConnected(boost::any value)
{
    if (value.type() != typeid(bool))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        bool isConnected = boost::any_cast<bool> (value);
        setIsConnected(isConnected);
    }
}

void SharedCommandData::extractIsSynched(boost::any value)
{
    if (value.type() != typeid(bool))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        bool isSynced = boost::any_cast<bool> (value);
        setIsSynched(isSynced);
    }
}

void SharedCommandData::extractVector2D(boost::any value)
{
	if (value.type() != typeid(vector2D))
	{
		Filter::setFilterError(filterError::INVALID_INPUT);
		Filter::setFilterStatus(filterStatus::FILTER_ERROR);
	}
	else
	{
		vector2D vec2D = boost::any_cast<vector2D> (value);
		setDelta(vec2D);
	}
}

void SharedCommandData::setDelta(vector2D delta)
{
	mouseDeltaMutex.lock();
	mouseDelta = delta;
	mouseDeltaMutex.unlock();
}

bool SharedCommandData::trySetDelta(vector2D delta)
{
	bool locked = mouseDeltaMutex.try_lock();
	if (locked) {
		mouseDelta = delta;
		mouseDeltaMutex.unlock();
	}

	return locked;
}

vector2D SharedCommandData::getDelta()
{
	mouseDeltaMutex.lock();
	vector2D delta = mouseDelta;
	mouseDeltaMutex.unlock();

	return delta;
}

bool SharedCommandData::tryGetDelta(vector2D& outDelta)
{
	bool locked = mouseDeltaMutex.try_lock();
	if (locked) {
		outDelta = mouseDelta;
		mouseDeltaMutex.unlock();
	}

	return locked;
}

void SharedCommandData::extractImpulseStatus(boost::any value)
{
    if (value.type() != typeid(bool))
    {
        Filter::setFilterError(filterError::INVALID_INPUT);
        Filter::setFilterStatus(filterStatus::FILTER_ERROR);
    }
    else
    {
        bool impulseStatus = boost::any_cast<bool> (value);
        setImpulseStatus(impulseStatus);
    }
}

void SharedCommandData::setImpulseStatus(bool impulseStatus)
{
    impulseStatusMutex.lock();
    this->impulseStatus = impulseStatus;
    impulseStatusMutex.unlock();
}

bool SharedCommandData::trySetImpulseStatus(bool impulseStatus)
{
    bool locked = impulseStatusMutex.try_lock();
    if (locked) {
        this->impulseStatus = impulseStatus;
        impulseStatusMutex.unlock();
    }

    return locked;
}

bool SharedCommandData::getImpulseStatus()
{
    impulseStatusMutex.lock();
    bool status = impulseStatus;
    impulseStatusMutex.unlock();

    return status;
}

bool SharedCommandData::tryGetImpulseStatus(bool& impulseStatus)
{
    bool locked = impulseStatusMutex.try_lock();
    if (locked) {
        impulseStatus = this->impulseStatus;
        impulseStatusMutex.unlock();
    }

    return locked;
}