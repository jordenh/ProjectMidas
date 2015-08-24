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

#include "GenericWindowMaxFilter.h"

GenericWindowMaxFilter::~GenericWindowMaxFilter()
{
}

void GenericWindowMaxFilter::process()
{
    filterDataMap input = Filter::getInput();

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::clearOutput();

    if (input.find(inputMapKey) != input.end())
    {
        boost::any value = input[inputMapKey];
        if (value.type() != typeid(int))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            processInt(boost::any_cast<int>(value));
        }
    }
}

void GenericWindowMaxFilter::processInt(int datum)
{
    insertElement(datum, dataDeque);

    filterDataMap output;

    output[inputMapKey] = calc(dataDeque);

    Filter::setOutput(output);
}

void GenericWindowMaxFilter::insertElement(int elem, std::deque<int>& dq)
{
    dq.push_back(elem);
    while (dq.size() > windowSize) {
        // discard element so that average is only based on
        // windowSize elements.
        dq.pop_front();
    }
}

int GenericWindowMaxFilter::calc(std::deque<int>& dq)
{
    int max = std::numeric_limits<int>::min();
    std::deque<int>::iterator it = dq.begin();
    while (it != dq.end())
    {
        if (*it > max)
        {
            max = *it;
        }
        it++;
    }
    return max;
}


