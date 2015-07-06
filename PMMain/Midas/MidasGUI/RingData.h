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

#ifndef RING_DATA_H
#define RING_DATA_H

#include <vector>

class ringData
{
public:
    ringData();
    ~ringData();

    ringData(const ringData& rd);

    struct keyboardValue
    {
        char main, hold;
        keyboardValue(char mainVal, char holdVal = '\0') : main(mainVal), hold(holdVal) { }
    };


    std::vector<keyboardValue>* getRingInVectorHandle();
    std::vector<keyboardValue>* getRingOutVectorHandle();

    void setRingInVector(std::vector<keyboardValue> ringInVal);
    void setRingOutVector(std::vector<keyboardValue> ringOutVal);



private:

    std::vector<keyboardValue> *ringInVector;
    std::vector<keyboardValue> *ringOutVector;


};

#endif RING_DATA_H
