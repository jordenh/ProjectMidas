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

#include "GenericBypassFilter.h"

GenericBypassFilter::GenericBypassFilter(std::string passThroughKey)
{
    std::vector<std::string> keyVec; 
    keyVec.push_back(passThroughKey);
    this->passThroughKeys = keyVec;
}

GenericBypassFilter::GenericBypassFilter(std::vector<std::string> passThroughKeys)
{
    this->passThroughKeys = passThroughKeys;
}

void GenericBypassFilter::process()
{
    filterDataMap output;
    filterDataMap input = Filter::getInput();

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::clearOutput();

    if (passThroughKeys.size() == 0)
    {
        output = input;
        Filter::setOutput(output);
    }
    else
    {
        for (int i = 0; i < passThroughKeys.size(); i++)
        {
            if (input.find(passThroughKeys[i]) != input.end())
            {
                output[passThroughKeys[i]] = input[passThroughKeys[i]];
            }
        }
        Filter::setOutput(output);
    }
}