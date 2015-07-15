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

#include "GenericAveragingFilter.h"

GenericAveragingFilter::~GenericAveragingFilter()
{
    int a = 1;
}

void GenericAveragingFilter::process()
{
    filterDataMap input = Filter::getInput();

    Filter::setFilterError(filterError::NO_FILTER_ERROR);
    Filter::clearOutput();

    if (input.find(inputMapKey) != input.end())
    {
        boost::any value = input[inputMapKey];
        if (value.type() != typeid(float))
        {
            Filter::setFilterError(filterError::INVALID_INPUT);
            Filter::setFilterStatus(filterStatus::FILTER_ERROR);
        }
        else
        {
            processFloat(boost::any_cast<float>(value));
        }
    }
}

void GenericAveragingFilter::processFloat(float datum)
{
    insertAvgElement(datum, dataDeque);

    filterDataMap output;

    output[inputMapKey] = calcAvg(dataDeque);

    Filter::setOutput(output);
}

void GenericAveragingFilter::insertAvgElement(float elem, std::deque<float>& dq)
{
    dq.push_back(elem);
    while (dq.size() > avgCount) {
        // discard element so that average is only based on
        // avgCount elements.
        dq.pop_front();
    }
}

float GenericAveragingFilter::calcAvg(std::deque<float>& dq)
{
    float sum = 0;
    float denom = dq.size();
    if (denom == 0)
    {
        return 0;
    }

    std::deque<float>::iterator it = dq.begin();
    while (it != dq.end())
    {
        sum += *it++;
    }
    return (float)sum / denom;
}


