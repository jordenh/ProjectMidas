#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include "DraggableWidget.h"

class QVBoxLayout;
class MidasThread;
class MouseIndicator;
class SequenceDisplayer;
class InfoIndicator;
class GestureSignaller;
class PoseDisplayer;
class ProfileIcon;
class ProfileDisplayer;
class ProfileSignaller;
class ProfileManager;
class SettingsDisplayer;
class SettingsSignaller;
#ifdef BUILD_KEYBOARD
class KeyboardWidget;
class DistanceWidget;
#endif

/**
 * The MainGUI class is the parent GUI of all the widgets used in Midas. It contains the mouse
 * indicator, sequence displayer, and info indicator. The mouse indicator shows the relative mouse 
 * velocity, the sequence displayer shows the gesture sequences, and the info indicator shows the
 * current status of Midas.
 */
class MainGUI : public DraggableWidget
{
    Q_OBJECT

public:
    /**
     * The constructor for the MainGUI.
     *
     * @param mainThread The main Midas thread; used to pass information between the GUI and back-end.
     * @param deadZoneRad The radius of the dead zone in the mouse indicator.
     */
    MainGUI(MidasThread *mainThread, ProfileManager* pm, int deadZoneRad);
    ~MainGUI();

    void connectSignallerToProfileWidgets(ProfileSignaller* signaller);

    void connectSignallerToInfoIndicator(GestureSignaller *signaller);
    void connectSignallerToSequenceDisplayer(GestureSignaller *signaller);
    void connectSignallerToPoseDisplayer(GestureSignaller *signaller);
	void connectSignallerToProfileIcons(GestureSignaller *signaller);

    void connectSignallerToSettingsDisplayer(SettingsSignaller *signaller);

#ifdef BUILD_KEYBOARD
public:
	void connectSignallerToKeyboardToggle(GestureSignaller *signaller);
public slots:
	void toggleKeyboard();
private:
	KeyboardWidget* keyboard;
//	DistanceWidget* distanceDisplayer;
#endif

private:

	void setupProfileIcons();

    QVBoxLayout *layout;
    MouseIndicator *mouseIndicator;
    InfoIndicator *infoIndicator;
    SequenceDisplayer *sequenceDisplayer;
    PoseDisplayer *poseDisplayer;
    SettingsDisplayer *settingsDisplayer;
	ProfileIcon *icon0;
	ProfileIcon *icon1;
	bool icon0IsActive;
    std::vector<ProfileDisplayer*> profileWidgets;
    int totalWidth, totalHeight;
    unsigned int numProfiles;
    unsigned int activeProfile;

public slots:
    void handleChangeProfile(bool progressForward);

    void handleFocusMidas();
};

#endif