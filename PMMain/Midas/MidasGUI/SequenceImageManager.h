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

#ifndef SEQUENCE_IMAGE_MANAGER_H
#define SEQUENCE_IMAGE_MANAGER_H

#include "MyoCommon.h"
#include <map>
#include <vector>

/* Rel. Image Paths */
#define FINGER_SPREAD_LATER_PATH  "Resources\\BW200PxImgs\\fingerSpread.jpg"
#define FINGER_SPREAD_NEXT_PATH   "Resources\\Post200PxImgs\\fingerSpread.jpg"
#define FIST_LATER_PATH           "Resources\\BW200PxImgs\\fist.jpg"
#define FIST_NEXT_PATH            "Resources\\Post200PxImgs\\fist.jpg"
//#define PINKY_TO_THUMB_LATER_PATH "Resources\\BW200PxImgs\\pinkyToThumb.jpg"
//#define PINKY_TO_THUMB_NEXT_PATH  "Resources\\Post200PxImgs\\pinkyToThumb.jpg"
#define DOUBLE_TAP_LATER_PATH     "Resources\\BW200PxImgs\\doubleTap.jpg"
#define DOUBLE_TAP_NEXT_PATH      "Resources\\Post200PxImgs\\doubleTap.jpg"
#define WAVE_IN_LATER_PATH        "Resources\\BW200PxImgs\\waveIn.jpg"
#define WAVE_IN_NEXT_PATH         "Resources\\Post200PxImgs\\waveIn.jpg"
#define WAVE_OUT_LATER_PATH       "Resources\\BW200PxImgs\\waveOut.jpg"
#define WAVE_OUT_NEXT_PATH        "Resources\\Post200PxImgs\\waveOut.jpg"
#define NO_HAND_LATER_PATH           "Resources\\Post200PxImgs\\noHand.jpg"
#define NO_HAND_NEXT_PATH            "Resources\\Post200PxImgs\\noHand.jpg"

#define POSE_LENGTH_IMMEDIATE_PATH "Resources\\icons\\PoseLenImmediate.png"
#define POSE_LENGTH_HOLD_PATH "Resources\\icons\\PoseLenHold.png"
#define POSE_LEN_SCALEDOWN_ICON 3

class SequenceImageManager
{
public:
    SequenceImageManager();
    ~SequenceImageManager();

    void loadImages();
    std::vector<sequenceImageSet> formSequenceSetFromIds(std::vector<int> ids, std::vector<PoseLength> lengths);

private:
    std::map<int, sequenceImageSet> idToImageMap;
};

#endif