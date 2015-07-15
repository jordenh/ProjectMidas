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

#include "KeyboardContoller.h"
#include "KeyboardVector.h"

INPUT* KeyboardController::convertKiVecToInputArr(KeyboardVector& kiVector)
{
	INPUT* kiArr = (INPUT*)std::malloc(sizeof(INPUT) * kiVector.peakVector().size());

	for (unsigned int i = 0; i < kiVector.peakVector().size(); i++)
	{
		kiArr[i].type = INPUT_KEYBOARD;
		kiArr[i].ki = kiVector.peakVector().at(i);
	}

	return kiArr;
}

int KeyboardController::sendData()
{
	INPUT* kiArr = convertKiVecToInputArr(kiVector);

	int numSent = SendInput(kiVector.peakVector().size(), kiArr, sizeof(INPUT));

	free(kiArr);
	
	return numSent;
}

int KeyboardController::sendDataDelayed(int milliDelay)
{
	INPUT* kiArr = convertKiVecToInputArr(kiVector);

	int numSent = 0;
	for (int i = 0; i < kiVector.peakVector().size(); i++)
	{
		numSent += SendInput(1, kiArr + i, sizeof(INPUT));
		Sleep(milliDelay);
	}

	free(kiArr);

	return numSent;
}