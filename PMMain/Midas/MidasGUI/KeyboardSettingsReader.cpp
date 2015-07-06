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

#include "KeyboardSettingsReader.h"
#include <boost/algorithm/string.hpp>
#include "ringData.h"
#include <fstream>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

KeyboardSettingsReader::KeyboardSettingsReader()
{
}


KeyboardSettingsReader::~KeyboardSettingsReader()
{
}


void KeyboardSettingsReader::readKeyboardSetupFile(std::vector<ringData>& ringDataHandle, std::string fileName)
{

    std::ifstream ringDataFile(fileName);
    std::vector<std::string> temp;
    std::string line;

    currentRing currentRing = ringOut;
    bool holdkeyCheck = false;

    ringData::keyboardValue keyTemp('\0');
    ringData *ringTemp = new ringData();

    if (ringDataFile.is_open())
    {
        while (!ringDataFile.eof())
        {
            getline(ringDataFile, line);
            boost::split(temp, line, std::bind2nd(std::equal_to<char>(), ' '));

            for (int i = 0; i < temp.size(); i++)
            {
                for (char & key : temp[i])
                {

                    if (temp[i].size() == 1) //size can only be 1(main) or 2(main & hold)
                    {
                        keyTemp.main = key;

                        if (currentRing == ringIn)
                        {
                            ringTemp->getRingInVectorHandle()->push_back(keyTemp);
                        }
                        else
                        {
                            ringTemp->getRingOutVectorHandle()->push_back(keyTemp);
                        }
                    }
                    else
                    {
                        if (holdkeyCheck == true)
                        {
                            keyTemp.hold = key;

                            if (currentRing == ringIn)
                            {
                                ringTemp->getRingInVectorHandle()->push_back(keyTemp);
                            }
                            else
                            {
                                ringTemp->getRingOutVectorHandle()->push_back(keyTemp);
                            }

                            holdkeyCheck = false;
                        }
                        else
                        {
                            keyTemp.main = key;
                            holdkeyCheck = true;
                        }
                    }
                }
            }

            if (currentRing == ringOut)
            {
                currentRing = ringIn;
            }
            else if (currentRing == ringIn)
            {
                currentRing = ringOut;
                ringDataHandle.push_back(*ringTemp);
                ringTemp = new ringData();
            }
        }
    }
    else
    {
        std::cout << "Error in opening file!";
    }
    ringDataFile.close();
    ringTemp->~ringData();
}
