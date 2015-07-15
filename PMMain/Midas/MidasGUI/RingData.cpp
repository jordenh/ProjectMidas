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

#include "RingData.h"
#include <vector>

ringData::ringData()
{
    this->ringInVector = new std::vector<keyboardValue>();
    this->ringOutVector = new std::vector<keyboardValue>();
}


ringData::~ringData()
{
    delete this->ringOutVector;
    delete this->ringInVector;
}

ringData::ringData(const ringData& rd)
{
    this->ringInVector = new std::vector<keyboardValue>(*rd.ringInVector);
    this->ringOutVector = new std::vector<keyboardValue>(*rd.ringOutVector);
}

std::vector<ringData::keyboardValue> *ringData::getRingInVectorHandle()
{
    return ringInVector;
}

std::vector<ringData::keyboardValue> *ringData::getRingOutVectorHandle()
{
    return ringOutVector;
}

void ringData::setRingInVector(std::vector<keyboardValue> ringInVal)
{
    *ringInVector = ringInVal;
}

void ringData::setRingOutVector(std::vector<keyboardValue> ringOutVal)
{
    *ringOutVector = ringOutVal;
}

