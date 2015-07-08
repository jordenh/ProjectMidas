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

#ifndef _MYO_COMMON_H
#define _MYO_COMMON_H

#include "myo\myo.hpp"
#include "MidasCommon.h"
#include <qpixmap.h>

#define CENTER_MAIN "Space"
#define CENTER_HOLD "Enter"
#define CENTER_MAIN_KEY 0x20
#define CENTER_HOLD_KEY 0x0D

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

class String;
class QLabel;

// Enumeration to distinguish between:
// TAP: 'short hold' ie, this is a pose that occurs for less time than REQ_HOLD_TIME,
//      and using this type allows holds to be registered that would logically conflict otherwise
// HOLD: indicates a pose needs to be held for REQ_HOLD_TIME to progress the sequence
// IMMEDIATE: this will progress a sequence immediately, but will mean that no holds of the same
//              pose-prefix can be registered without conflict. This should be okay, as it's main 
//              design intention is to allow for click/drag functionality.
enum class PoseLength {
    TAP,
    HOLD,
    IMMEDIATE
};

/*
* Each step in a sequence may have three different images, depending on
* the progress of the sequence. The 'doneImage' is displayed on sequence
* steps that have already been performed; the 'nextImage' is displayed for
* the next step that must be performed, and the 'laterImage' applies to all
* other steps.
*/
struct sequenceImageSet
{
    QPixmap nextImage, laterImage;
    QLabel* currentImgLabel;
    int actionTag;
    PoseLength poseLen;
};

struct sequenceData
{
    QLabel *seqLabel, *seqPosLabel;
    int imageOffset;
    std::vector<sequenceImageSet> sequenceImages;
};

struct sequenceProgressData
{
    sequenceProgressData()
    {
        seqId = progress = 0;
    }

    sequenceProgressData(int id, int prog)
    {
        seqId = id;
        progress = prog;
    }

    int seqId;
    int progress;
};

/**
* Simple string conversion for Pose::Types to string.
*
* @param gesture The gesture to get toString() of.
* @return std::string The string equivelant of the input gesture.
*/
std::string PoseTypeToString(Pose::Type gesture);

struct SeqElement {
    SeqElement(Pose::Type type) {
        this->type = type;
        poseLen = PoseLength::TAP;
    }
    SeqElement(Pose::Type type, PoseLength PL) {
        this->type = type;
        this->poseLen = PL;
    }

    Pose::Type type;

    PoseLength poseLen = PoseLength::TAP; 

    bool operator==(SeqElement& e)
    {
        if (e.type == type && e.poseLen == poseLen)
        {
            return true;
        }
        return false;
    }
    
    bool operator!=(SeqElement& e)
    {
        return !(e == *this);
    }
};

// If the number of buzz modes change, all buzzMode logic will need to change, so must find all usages and re-verify.
#define NUM_BUZZ_MODES 4
enum buzzFeedbackMode {
    NO_BUZZ,
    MINIMAL, // only on unlock/lock and profile changes.
    ALLSTATECHANGES,
    ALLACTIONS
};
static std::string buzzFeedbackModeToString(buzzFeedbackMode bfm)
{
    switch (bfm)
    {
    case NO_BUZZ:
        return "No Buzz";
    case MINIMAL:
        return "Minimal Buzz";
    case ALLSTATECHANGES:
        return "State-Change Buzz";
    case ALLACTIONS:
        return "All-Action Buzz";
    default:
        return "default";
    }
}

#endif /* _MYO_COMMON_H */