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
    inputData = input;
}

filterDataMap Filter::getOutput()
{
    return outputData;
}

filterStatus Filter::getFilterStatus()
{
    return status;
}

filterError Filter::getFilterError()
{
    return error;
}

void Filter::setFilterStatus(filterStatus status)
{
    this->status = status;
}

void Filter::setFilterError(filterError error)
{
    this->error = error;
}

filterDataMap Filter::getInput()
{
    return inputData;
}

void Filter::setOutput(filterDataMap output)
{
    outputData = output;
}

void Filter::addToOutput(filterDataMap output)
{
    outputData = joinFilterDataMaps(outputData, output);
}

void Filter::clearOutput(void)
{
    outputData = filterDataMap();
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