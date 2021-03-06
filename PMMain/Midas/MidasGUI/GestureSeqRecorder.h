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

#ifndef _GESTURE_SEQ_RECORDER_H
#define _GESTURE_SEQ_RECORDER_H


#include "CommandData.h"
#include "myo\myo.hpp"
#include "GestureSignaller.h"
#include "SettingsSignaller.h"
#include "SequenceImageManager.h"
#include <ctime>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <mutex>

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

class ControlState;
class MainGUI;

enum class SequenceStatus {
    SUCCESS,
    CONFLICTING_SEQUENCE,
    INVALID_SEQUENCE,
    UNEXPECT_STATE_CHANGE
};

typedef std::vector<SeqElement> sequence;

/**
* Wrapper to tie state information to a sequence response.
*/
struct sequenceInfo {
    sequenceInfo() {
        progress = 0;
        id = counter++;
    }

    sequence seq;
    CommandData sequenceResponse;
    static unsigned int counter;
    unsigned int progress;
    unsigned int id;
    std::string sequenceName;
};

typedef std::list<sequenceInfo> sequenceList;
typedef std::map<midasMode, sequenceList*> sequenceMapPerMode;

class GestureSeqRecorder
{
public:
    /**
    * Constructors/Destructor
    */
    GestureSeqRecorder(ControlState* controlStateHandle, MainGUI* mainGuiHandle, SequenceImageManager imageManager);
    ~GestureSeqRecorder();

    void unregisterAll();

    /**
    * Register a sequence of gestures with this call. A registered sequence is associated with a 
    * midasMode. When in that mode, any progression of gestures from the Myo will be compared
    * to all associated and registered sequences, and if any are succesfully completed, the 
    * registered CommandData will be returned to the caller.
    * NOTE: Can only register sequences with SeqElements of PoseLength IMMEDIATE if the sequence
    * is of length one. This could be changed in the future if deemed necessary, but this allows
    * progressSequence to be unnafected by this special case.
    *
    * @param mode The midasMode that the caller is registering the sequence against.
    * @param seq The sequence (std::vector<Pose::Type>) of gestures to register
    * @param seqResponse The desired response to activate if the sequence is recognized
    * while in the registered mode.
    * @return SequenceStatus associated status information to inform caller of success/lack there of
    */
    SequenceStatus registerSequence(midasMode mode, sequence seq, CommandData seqResponse, std::string name);

    /**
    * Given a gesture, attempt to progress through any registered sequences, that match the mode
    * of the controlState.
    * 
    * @param gesture The recorded gesture that is being used to compare with the registered sequences.
    * @param state The ControlState handle to determine the current midasMode of the system, thereby 
    * knowing which registered sequence list to search through.
    * @param response The CommandData that is populated by the function. Holding a type of NONE
    * means that no sequence was completed. However, if it's not NONE, it holds the response that
    * was registered against the completed sequence.
    * @return SequenceStatus associated status information to inform caller of success/lack there of
    */ 
    SequenceStatus progressSequence(Pose::Type gesture, ControlState state, CommandData& response);

    /**
    * To handle tap/hold differentiation, this should be called to notify the SeqRecorder that a 
    * certain amount of time has passed. If a certain amount of time passes while a user is holding
    * a specific pose, then they are deemed to be holding it, otherwise they have tapped it.
    * This function will continually decrement the hold timer, and if it reaches 0, it will clear
    * any active sequences that were supposed to have a 'tap' action, and will progress any sequences
    * that have a 'hold' action.
    *
    * @param delta The amount of time in ms indicated to have passed.
    * @param response The CommandData that is populated by the function. Holding a type of NONE
    * means that no sequence was completed. However, if it's not NONE, it holds the response that
    * was registered against the completed sequence.
    * @return SequenceStatus The status of the progression. SUCCESS is typical and wanted.
    */
    void progressSequenceTime(int delta, CommandData& response);

    /**
    * Called to check against progressBaseTime if any sequences are active, so that a 
    * sequence can be cleared if timed out, without an asynchronous gesture update.
    */
    void checkProgressBaseTime();

    /**
    * Empty the references to sequences that were active, and set progress of each to 0.
    */
    void emptyActiveSequences();

    /**
    * Modifier.
    *
    * @param newTime The new progressMaxDeltaTime.
    */
    void setProgressMaxDeltaTime(clock_t newTime);

    /**
    * Accessor.
    *
    * @return Value of progressMaxDeltaTime.
    */
    clock_t getProgressMaxDeltaTime(void);

    /**
    * Print out a list of all active sequences, and possibly their progress,
    * and next gesture required to proceed!
    *
    * @param verbose If true, prints progress and next gesture in sequence on top
    * of active status.
    */
    void printStatus(bool verbose = false);

    /**
    * Updates the sequence displayer GUI with the latest sequence information. This should be
    * called upon any update to the sequences.
    */
    void updateGuiSequences();

private:
    SequenceStatus checkLegalRegister(midasMode mode, sequenceInfo seqInfo) const;

    /**
    * Track state information when attempting to start a sequence. Then ensure 
    * that state isn't altered until the end of a sequence, or else something
    * has become corrupt, so cleanup and return an error.
    *
    * @param state The state to verify the associated mode has persisted throughout 
    * an active sequence cycle.
    * @return SequenceStatus associated status information to inform caller of success/lack there of
    */
    SequenceStatus ensureSameState(ControlState state);

    /**
    * Perform the duties of progressSequence, but optimized for the situation where one or more sequences
    * have already been noted as active.
    *
    * @param x Same as progressSequence.
    * @return x Same as progressSequence.
    */
    SequenceStatus progressActiveSequences(Pose::Type gesture, ControlState state, CommandData& response);

    /**
    * Perform the duties of progressSequence, but optimized for the situation where no sequence has yet
    * been deemed active.
    *
    * @param x Same as progressSequence.
    * @return x Same as progressSequence.
    */
    SequenceStatus findActivation(Pose::Type gesture, ControlState state, CommandData& response);

    /**
     * Loads the images needed for the sequence GUI and connects the signals of the GestureSignaller to the
     * slots of the GUI.
     */
    void connectGuiSignals();

    // Holds all registered CommandDatas in a layered organization.
    sequenceMapPerMode *seqMapPerMode; // owned and memory handled

    // Stores pointers to active sequenceInfos, so that progress can be tracked more efficiently.
    // Once ANY sequences are active, ONLY those sequences can potentially progress, until they 
    // are timed out.
    std::list<sequenceInfo*> activeSequences; // not owned

    std::mutex activeSequencesMutex;

    // State info from when a sequence that has size >1 is started.
    midasMode prevState;

    Pose::Type prevPose;

    bool prevShowAll;
    midasMode timeBasedPrevState; // previous state seen in time, used for GUI purposes.

    // Base timestamp used to calculate transitions
    clock_t progressBaseTime;

    // Transition timeout value. For example, if to change a state,
    // the user is required to perform "wave in, wave out, double-tap", 
    // then when a wave in is recorded, the user has up to "TransMaxDeltaTime"
    // milliseconds to perform a wave out, or else the whole process needs to be repeated.
    clock_t progressMaxDeltaTime;

    // Timer to hold the amount of time on each progression of a sequence, which 
    // will determine if a user tapped a pose, or held it.
    clock_t holdGestTimer;

    ControlState* controlStateHandle; // not owned

    SequenceImageManager imageManager;
    MainGUI* mainGui; // not owned
    GestureSignaller gestureSignaller;
    static SettingsSignaller settingsSignaller;
};

#endif /* _GESTURE_SEQ_RECORDER_H */