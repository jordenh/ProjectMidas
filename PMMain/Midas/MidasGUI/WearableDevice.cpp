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

#include "WearableDevice.h"

#include "SharedCommandData.h"
#include <thread>

static void wearableThreadWrapper(WearableDevice* wearableDevice);

deviceStatus WearableDevice::getDeviceStatus()
{
    deviceStatus statusReturn;
    statusLock.lock();
    statusReturn = status;
    statusLock.unlock();
    return statusReturn;
}

void WearableDevice::setDeviceStatus(deviceStatus newStatus)
{
    statusLock.lock();
    status = newStatus;
    statusLock.unlock();
}

void WearableDevice::stopDevice()
{
    stopFlagMutex.lock();
    stopFlag = true;
    stopFlagMutex.unlock();
}

bool WearableDevice::stopDeviceRequested()
{
    bool flagResult;
    stopFlagMutex.lock();
    flagResult = stopFlag;
    stopFlagMutex.unlock();
    return flagResult;
}

/*
 * This kicks off the thread that runs the wearable device's main loop. This loop is
 * used to collect information from the device and pass actions (commands, coordinated of the mouse)
 * to the shared data (which wearableDevice has a reference to).
 */
void startWearableDeviceListener(WearableDevice* wearableDevice)
{
    std::thread devThread(wearableThreadWrapper, wearableDevice);
    devThread.detach();
}

void wearableThreadWrapper(WearableDevice* wearableDevice)
{
    wearableDevice->runDeviceLoop();

    do {
        std::this_thread::yield();
    } while (true);
}

