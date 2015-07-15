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

/* Relative Image Paths */
#define RIGHT_FINGER_SPREAD_LATER_PATH   "Resources\\assets\\gesture_icons\\solid_grey_RH_spread_fingers.png"         // "Resources\\BW200PxImgs\\fingerSpread.jpg"
#define RIGHT_FINGER_SPREAD_NEXT_PATH    "Resources\\assets\\gesture_icons\\blue_outline_RH_spread_fingers.png"       //"Resources\\Post200PxImgs\\fingerSpread.jpg"
#define RIGHT_FIST_LATER_PATH            "Resources\\assets\\gesture_icons\\solid_grey_RH_fist.png"                   //"Resources\\BW200PxImgs\\fist.jpg"
#define RIGHT_FIST_NEXT_PATH             "Resources\\assets\\gesture_icons\\blue_outline_RH_fist.png"                 //"Resources\\Post200PxImgs\\fist.jpg"
#define RIGHT_DOUBLE_TAP_LATER_PATH      "Resources\\assets\\gesture_icons\\solid_grey_RH_double_tap.png"             //"Resources\\BW200PxImgs\\doubleTap.jpg"
#define RIGHT_DOUBLE_TAP_NEXT_PATH       "Resources\\assets\\gesture_icons\\blue_outline_RH_double_tap.png"           //"Resources\\Post200PxImgs\\doubleTap.jpg"
#define RIGHT_WAVE_IN_LATER_PATH         "Resources\\assets\\gesture_icons\\solid_grey_RH_wave_left.png"              //"Resources\\BW200PxImgs\\waveIn.jpg"
#define RIGHT_WAVE_IN_NEXT_PATH          "Resources\\assets\\gesture_icons\\blue_outline_RH_wave_left.png"            //"Resources\\Post200PxImgs\\waveIn.jpg"
#define RIGHT_WAVE_OUT_LATER_PATH        "Resources\\assets\\gesture_icons\\solid_grey_RH_wave_right.png"             //"Resources\\BW200PxImgs\\waveOut.jpg"
#define RIGHT_WAVE_OUT_NEXT_PATH         "Resources\\assets\\gesture_icons\\blue_outline_RH_wave_right.png"           //"Resources\\Post200PxImgs\\waveOut.jpg"
#define LEFT_FINGER_SPREAD_LATER_PATH    "Resources\\assets\\gesture_icons\\solid_grey_LH_spread_fingers.png"         // "Resources\\BW200PxImgs\\fingerSpread.jpg"
#define LEFT_FINGER_SPREAD_NEXT_PATH     "Resources\\assets\\gesture_icons\\blue_outline_LH_spread_fingers.png"       
#define LEFT_FIST_LATER_PATH             "Resources\\assets\\gesture_icons\\solid_grey_LH_fist.png"                   
#define LEFT_FIST_NEXT_PATH              "Resources\\assets\\gesture_icons\\blue_outline_LH_fist.png"                 
#define LEFT_DOUBLE_TAP_LATER_PATH       "Resources\\assets\\gesture_icons\\solid_grey_LH_double_tap.png"             
#define LEFT_DOUBLE_TAP_NEXT_PATH        "Resources\\assets\\gesture_icons\\blue_outline_LH_double_tap.png"           
#define LEFT_WAVE_IN_LATER_PATH          "Resources\\assets\\gesture_icons\\solid_grey_LH_wave_right.png"              
#define LEFT_WAVE_IN_NEXT_PATH           "Resources\\assets\\gesture_icons\\blue_outline_LH_wave_right.png"            
#define LEFT_WAVE_OUT_LATER_PATH         "Resources\\assets\\gesture_icons\\solid_grey_LH_wave_left.png"             
#define LEFT_WAVE_OUT_NEXT_PATH          "Resources\\assets\\gesture_icons\\blue_outline_LH_wave_left.png"           
#define NO_HAND_LATER_PATH               "Resources\\assets\\gesture_icons\\grey.png"                                 //"Resources\\Post200PxImgs\\noHand.jpg"
#define NO_HAND_NEXT_PATH                "Resources\\assets\\gesture_icons\\blue.png"                                 //"Resources\\Post200PxImgs\\noHand.jpg"

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