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

#pragma once
#include <vector>
#include <Windows.h>
#include "MidasCommon.h"

#define NO_VK_REP_FOUND -1

class KeyboardVector {
public:
	void clear();

	void inputCharDown(char c);

	void inputCharUp(char c);

	void inputCharDownUp(char c);

	void inputVKDown(unsigned int vk);

	void inputVKUp(unsigned int vk);

	void inputVKDownUp(unsigned int vk);

	void mirrorVectorAllUp();

	const std::vector<KEYBDINPUT> peakVector() { return kiVector; }

	static KeyboardVector createFromCommand(kybdCmds cmd);

	static KeyboardVector createFromProfileStr(std::string str);

private:
	unsigned int tryInsertCharAsVK(char c);

	std::vector<KEYBDINPUT> kiVector;
};