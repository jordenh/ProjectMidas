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

#include "Filter.h"

#include "ProfileManager.h"

void Filter::addDataAsInput(std::string name, boost::any value)
{
    inputData[name] = value;
}

void Filter::setInput(filterDataMap input)
{
    filterMutex.lock();
    inputData = input;
    filterMutex.unlock();
}

filterDataMap Filter::getOutput()
{
    filterMutex.lock();
    filterDataMap retVal = outputData;
    filterMutex.unlock();
    return retVal;
}

filterStatus Filter::getFilterStatus()
{
    filterMutex.lock();
    filterStatus retVal = status;
    filterMutex.unlock();
    return retVal;
}

filterError Filter::getFilterError()
{
    filterMutex.lock();
    filterError retVal = error;
    filterMutex.unlock();
    return retVal;
}

void Filter::setFilterStatus(filterStatus status)
{
    filterMutex.lock();
    this->status = status;
    filterMutex.unlock();
}

void Filter::setFilterError(filterError error)
{
    filterMutex.lock();
    this->error = error;
    filterMutex.unlock();
}

filterDataMap Filter::getInput()
{
    filterMutex.lock();
    filterDataMap retVal = inputData;
    filterMutex.unlock();
    return retVal;
}

void Filter::setOutput(filterDataMap output)
{
    filterMutex.lock();
    outputData = output;
    filterMutex.unlock();
}

void Filter::addToOutput(filterDataMap output)
{
    filterMutex.lock();
    outputData = joinFilterDataMaps(outputData, output);
    filterMutex.unlock();
}

void Filter::clearOutput(void)
{
    filterMutex.lock();
    outputData = filterDataMap();
    filterMutex.unlock();
}

filterError Filter::updateBasedOnProfile(ProfileManager& pm, std::string name)
{
    return filterError::NO_FILTER_ERROR;
}

filterDataMap Filter::joinFilterDataMaps(filterDataMap map0, filterDataMap map1)
{
    filterDataMap result = map0;
    result.insert(map1.begin(), map1.end());

    return result;
}

bool Filter::mapCollision(filterDataMap map0, filterDataMap map1)
{
    for (std::map<std::string, boost::any>::iterator map0It = map0.begin(); map0It != map0.end(); map0It++)
    {
        if (map1.find((*map0It).first) != map1.end())
        {
            return true;
        }
    }

    filterDataMap result = map0;
    result.insert(map1.begin(), map1.end());

    return false;
}