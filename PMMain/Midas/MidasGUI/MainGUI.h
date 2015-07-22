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

#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include "DraggableWidget.h"

class QVBoxLayout;
class QFrame;
class MidasThread;
class MouseIndicator;
class SequenceDisplayer;
class InfoIndicator;
class GestureSignaller;
class ConnectionSignaller;
class PoseDisplayer;
class ProfileIcon;
class ProfileDisplayer;
class ProfileSignaller;
class ProfileManager;
class SettingsDisplayer;
class SettingsSignaller;
class KeyboardWidget;
class DistanceWidget;
class ProfilesDisplayer;

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
    void connectSignallerToSequenceDisplayer(ConnectionSignaller *signaller);
    void connectSignallerToPoseDisplayer(GestureSignaller *signaller);
    void connectSignallerToPoseDisplayer(ConnectionSignaller *signaller);
	void connectSignallerToProfileIcons(GestureSignaller *signaller);

    void connectSignallerToSettingsDisplayer(SettingsSignaller *signaller);

	void connectSignallerToKeyboardToggle(GestureSignaller *signaller);
	
    void startShowGuiThread();

private:
    void hideAllNonEssentialWidgets();

    void setupProfileIcons();

    void toggleSettingsDisplayer();
    void toggleProfileDisplayer();

    // on a timer, call this->show() to ensure Midas stays on top even if other applications have 
    // alerts/popups that temporarily steal focus.
    void showGuiThread();

	KeyboardWidget* keyboard;
	DistanceWidget* distanceDisplayer;

    //MouseIndicator *mouseIndicator;
    InfoIndicator *infoIndicator;
    SequenceDisplayer *sequenceDisplayer;
    PoseDisplayer *poseDisplayer;
    SettingsDisplayer *settingsDisplayer;
	ProfileIcon *icon0;
	ProfileIcon *icon1;
	bool icon0IsActive;
    ProfilesDisplayer *profilesWidget;
    int totalWidth, totalHeight;
    unsigned int numProfiles;
    unsigned int activeProfile;

public slots:
    void toggleKeyboard();

    void handleChangeProfile(bool progressForward);

    void handleFocusMidas();

private slots:
    void ShowContextMenu(const QPoint& pos);

};

#endif