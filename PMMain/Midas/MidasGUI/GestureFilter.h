#ifndef _GESTURE_FILTER_H
#define _GESTURE_FILTER_H

#include "Filter.h"
#include "GestureSignaller.h"
#include "SettingsSignaller.h"
#include "SequenceImageManager.h"
#include <ctime>

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
#include "myo\myo.hpp"
using namespace myo;
#endif

class CommandData;
class ControlState;
class MyoState;
class GestureSeqRecorder;
class MainGUI;

#define GESTURE_INPUT "gesture"
#define MYO_GESTURE_RIGHT_MOUSE Pose::fingersSpread
#define MYO_GESTURE_LEFT_MOUSE Pose::fist

// GestureFilter spawns a thread to execute callback functions at this
// period.
#define SLEEP_LEN 20 // ms



/**
 * Consult Filter.h for concepts regarding Filters.
 * 
 * A filter specific to the Myo armband that handles changing the application
 * state when a certain gesture is performed, and translates from Myo gestures
 * to mouse and keyboard commands. Furthermore, helps to prevent against false 
 * positive gesture recognition by forcing the user to hold a gesture for a
 * certain amount of time before registering it.
 */
class GestureFilter : public Filter
{
public:
    /**
     * The constructor for the GestureFilter. It takes a ControlState handle so
     * that the state of the application can be changed by gesture control. It sets
     * the time that must occur before a gesture is registered.
     *
     * @param controlState A handle to the ControlState object that manages application state.
     * @param timeDel The time that a user must hold a gesture before it is registered.
     */
	GestureFilter(ControlState* controlState, MyoState* myoState, clock_t timeDel, MainGUI *mainGuiHandle);
    ~GestureFilter();

    /**
     * This is the process() function that overrides the base Filter class process(). It handles 
     * the logic of the filter. It translates gestures to the corresponding mouse and keyboard commands,
     * handles changing the application state, and enforces the timing restriction that prevents, or rather
     * reduces the chance of, false positive gestures from being recognized.
     */
    void process();

    filterDataMap getExtraDataForSCD();

    filterError updateBasedOnProfile(ProfileManager& pm, std::string name);

    /**
    * return actual handle to gestSeqRecorder
    */
    GestureSeqRecorder *getGestureSeqRecorder() { return gestSeqRecorder; }

    static void handleStateChange(CommandData response, GestureFilter *gf);
	static void handleProfileChangeCommand(CommandData response, GestureFilter *gf);

    friend void setupCallbackThread(GestureFilter *gf);
    friend void callbackThreadWrapper(GestureFilter *gf);

private:
    /**
     * Translates gestures into corresponding mouse and keyboard commands.
     *
     * @param pose The pose to translate into a command.
     */
    CommandData translateGesture(Pose::Type pose);

    /**
    * Performs initial registration of Mouse Sequences as a default incase profile manager
    * has no profiles to populate with.
    */
    void registerMouseSequences(void);

    /**
    * Performs initial registration of Keyboard Sequences as a default incase profile manager
    * has no profiles to populate with.
    */
    void registerKeyboardSequences(void);

    /**
    * Performs initial registration of State Sequences as a default incase profile manager
    * has no profiles to populate with.
    */
    void registerStateSequences(void);

    /**
    * Given a completed sequence, returning a CommandData response, this handles the response
    * as a Mouse command, populating a filterDataMap which is passed down the filter chain and
    * returned.
    *
    * @param response The CommandData retrieved from a completed sequence to be handled
    * @return the populated filterDataMap from the CommandData 
    */
	filterDataMap handleMouseCommand(CommandData response);

    /**
    * Given a completed sequence, returning a CommandData response, this handles the response
    * as a Keyboard command, populating a filterDataMap which is passed down the filter chain and
    * returned.
    *
    * @param response The CommandData retrieved from a completed sequence to be handled
    * @parfam addToExtra If true, the generated filterDataMap will be set as extra data
    * that the SCD will consume on it's next iteration
    * @return the populated filterDataMap from the CommandData
    */
	filterDataMap handleKybrdCommand(CommandData response, bool addToExtra = false);

    /**
    * Given a completed sequence, returning a CommandData response, this handles the response
    * as a Profile Change command, populating a filterDataMap which is passed down the filter chain and
    * returned.
    *
    * @param response The CommandData retrieved from a completed sequence to be handled
    * @return the populated filterDataMap from the CommandData
    */
	filterDataMap handleProfileChangeCommand(CommandData response);

    /**
    * Emit a pose value from an owned signaller, to relay to the GUI thread
    *
    * @param poseInt The int corresponding to the desired Pose ID to be reflected
    * on the GUI
    */
    void emitPoseData(int poseInt);

    filterDataMap extraDataForSCD;

    Pose::Type lastPoseType;
    
    static ControlState* controlStateHandle;
	MyoState* myoStateHandle;
    clock_t timeDelta;
    clock_t lastTime;

    GestureSeqRecorder* gestSeqRecorder;

    MainGUI *mainGui;
    static GestureSignaller gestureSignaller;
    static SettingsSignaller settingsSignaller;

    SequenceImageManager imageManager;
};

#endif /* _GESTURE_FILTER_H */