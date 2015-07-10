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

#include "MyoCommon.h"

#include <string>
#include <QLabel.h>

std::string PoseTypeToString(Pose::Type gesture)
{
    if (gesture == Pose::Type::fingersSpread)
    {
        return "fingersSpread";
    }
    else if (gesture == Pose::Type::fist)
    {
        return "fist";
    }
    else if (gesture == Pose::Type::rest)
    {
        return "rest";
    }
    else if (gesture == Pose::Type::doubleTap)
    {
        return "doubleTap";
    }
    else if (gesture == Pose::Type::waveIn)
    {
        return "waveIn";
    }
    else if (gesture == Pose::Type::waveOut)
    {
        return "waveOut";
    }
    else if (gesture == Pose::Type::unknown)
    {
        return "unknown";
    }
    else
    {
        return "ErrorInMyoCommonToString";
    }
}
